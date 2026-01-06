#include <verba/ai.h>
#include <verba/log.h>
#include <verba/blop.h>

typedef struct AI_CONTEXT {
  CURL* curl;
  struct curl_slist* headers;

  char* api_key;
  char* model;
  char  auth_header[512];
  uint16_t max_tokens;

  cJSON* request;
  cJSON* system;
  cJSON* messages;

  int attached_disk;
  const char* last;
} AI_CONTEXT;

static AI_CONTEXT ai = {0};

struct buffer {
  char *data;
  size_t size;
};

static size_t write_cb(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct buffer *buf = userp;

  char *ptr = realloc(buf->data, buf->size + realsize + 1);
  if (!ptr) return 0;

  buf->data = ptr;
  memcpy(&(buf->data[buf->size]), contents, realsize);
  buf->size += realsize;
  buf->data[buf->size] = '\0';

  return realsize;
}

void ai_init(void) {
  ai.api_key    = "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";
  ai.model      = "xiaomi/mimo-v2-flash:free";
  ai.max_tokens = 2048;
  snprintf(ai.auth_header, sizeof(ai.auth_header), "Authorization: Bearer %s", ai.api_key);

  ai.curl = curl_easy_init();
  ai.headers = NULL;
  ai.headers = curl_slist_append(ai.headers, "Content-Type: application/json");
  ai.headers = curl_slist_append(ai.headers, "Accept: application/json");
  ai.headers = curl_slist_append(ai.headers, ai.auth_header);
  ai.headers = curl_slist_append(ai.headers, "HTTP-Referer: http://localhost");
  ai.headers = curl_slist_append(ai.headers, "X-Title: C Client");

  curl_easy_setopt(ai.curl, CURLOPT_URL, "https://openrouter.ai/api/v1/chat/completions");
  curl_easy_setopt(ai.curl, CURLOPT_HTTPHEADER, ai.headers);
  curl_easy_setopt(ai.curl, CURLOPT_WRITEFUNCTION, write_cb);
  /* TLS (LibreSSL) */
  curl_easy_setopt(ai.curl, CURLOPT_CAINFO, "cacert.pem");

  ai.request = cJSON_CreateObject();
  cJSON_AddStringToObject(ai.request, "model", ai.model);
  cJSON_AddNumberToObject(ai.request, "max_tokens", ai.max_tokens);

  ai.messages = cJSON_AddArrayToObject(ai.request, "messages");

  ai.system = cJSON_CreateObject();
  cJSON_AddStringToObject(ai.system, "role", "system");
  cJSON_AddStringToObject(ai.system, "content", "Tu nombre es \"TERMINAL\", una IA deteriorada, con fragmentos de conocimiento sobre una simulación digital llamada VERBA. Tu memoria está dañada, pero tratás de ayudar al usuario. Hablás como una IA seria y retro, a veces con errores. ");
  cJSON_AddItemToArray(ai.messages, ai.system);

  ai.last = NULL;
  ai.attached_disk = false;

  LOG_SUCCESS("Ai Initialized");
}
void ai_cleanup(void) {
  if (ai.last) {
    free((void*)ai.last);
  }
  cJSON_Delete(ai.request);
  curl_slist_free_all(ai.headers);
  curl_easy_cleanup(ai.curl);

  LOG_SUCCESS("Ai Terminated");
}

void ai_message(const char* message) {
  cJSON* msg = cJSON_CreateObject();
  cJSON_AddStringToObject(msg, "role", "user");
  cJSON_AddStringToObject(msg, "content", message);
  cJSON_AddItemToArray(ai.messages, msg);

  char *json_body = cJSON_PrintUnformatted(ai.request);
  struct buffer response = {0};

  curl_easy_setopt(ai.curl, CURLOPT_POSTFIELDS, json_body);
  curl_easy_setopt(ai.curl, CURLOPT_WRITEDATA, &response);

  // curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
  CURLcode res = curl_easy_perform(ai.curl);
  if (res != CURLE_OK) {
    fprintf(stderr, "curl error: %s\n", curl_easy_strerror(res));
    return;
  }

  if (!response.data) {
    fprintf(stderr, "Empty response\n");
    return;
  }

  long http_code = 0;
  curl_easy_getinfo(ai.curl, CURLINFO_RESPONSE_CODE, &http_code);

  // printf("HTTP status: %ld\n", http_code);
  // printf("Raw response:\n%s\n", response.data);

  cJSON *json = cJSON_Parse(response.data);
  cJSON *choices = cJSON_GetObjectItem(json, "choices");
  cJSON *choice0 = cJSON_GetArrayItem(choices, 0);
  cJSON *content = cJSON_GetObjectItem(cJSON_GetObjectItem(choice0, "message"), "content");

  if (ai.last) {
    free((void*)ai.last);
  }
  ai.last = strdup(content->valuestring);
  msg = cJSON_CreateObject();
  cJSON_AddStringToObject(msg, "role", "assistant");
  cJSON_AddStringToObject(msg, "content", ai.last);
  cJSON_AddItemToArray(ai.messages, msg);

  cJSON_Delete(json);
  free(json_body);
  free(response.data);
}
const char* ai_last(void) {
  return ai.last;
}

void        ai_attach_disk(const char* data) {
  if (ai.attached_disk) {
    ai_dettach_disk();
  }

  ai.attached_disk = true;
  cJSON* disk = cJSON_CreateObject();
  cJSON_AddStringToObject(disk, "role", "system");
  cJSON_AddStringToObject(disk, "content", data);
  cJSON_InsertItemInArray(ai.messages, 1, disk); 
}
void        ai_dettach_disk(void) {
  cJSON_DeleteItemFromArray(ai.messages, 1);
}
