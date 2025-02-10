#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <microhttpd.h>

#define PORT 5000
#define DIRECTORY "/home/pi/Desktop/shared"

int send_response(struct MHD_Connection *connection, const char *content, int status_code) {
    struct MHD_Response *response;
    int ret;

    response = MHD_create_response_from_buffer(strlen(content), (void *)content, MHD_RESPMEM_MUST_COPY);
    if (!response) return MHD_NO;

    ret = MHD_queue_response(connection, status_code, response);
    MHD_destroy_response(response);

    return ret == MHD_YES ? MHD_YES : MHD_NO;
}

enum MHD_Result request_handler(void *cls, struct MHD_Connection *connection, const char *url,
                                const char *method, const char *version, const char *upload_data,
                                size_t *upload_data_size, void **con_cls) {
    char response_buffer[8192] = {0};

    if (strcmp(method, "GET") != 0) {
        return send_response(connection, "{\"error\":\"Method Not Allowed\"}", MHD_HTTP_METHOD_NOT_ALLOWED);
    }

    if (strcmp(url, "/files") == 0) {
        DIR *dir;
        struct dirent *entry;
        strcat(response_buffer, "{\"files\":[");

        if ((dir = opendir(DIRECTORY)) != NULL) {
            int first = 1;
            while ((entry = readdir(dir)) != NULL) {
                if (entry->d_type == DT_REG) {
                    if (!first) strcat(response_buffer, ",");
                    first = 0;
                    strcat(response_buffer, "\"");
                    strcat(response_buffer, entry->d_name);
                    strcat(response_buffer, "\"");
                }
            }
            closedir(dir);
        }
        strcat(response_buffer, "]}");
        return send_response(connection, response_buffer, MHD_HTTP_OK);
    } 
    
    else if (strncmp(url, "/files/", 7) == 0) {
        char file_path[512];
        snprintf(file_path, sizeof(file_path), "%s/%s", DIRECTORY, url + 7);

        FILE *file = fopen(file_path, "rb");
        if (!file) {
            return send_response(connection, "{\"error\":\"File not found\"}", MHD_HTTP_NOT_FOUND);
        }

        struct stat file_stat;
        stat(file_path, &file_stat);
        char *file_content = malloc(file_stat.st_size);
        fread(file_content, 1, file_stat.st_size, file);
        fclose(file);

        struct MHD_Response *response = MHD_create_response_from_buffer(file_stat.st_size, file_content, MHD_RESPMEM_MUST_FREE);
        int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
        return ret == MHD_YES ? MHD_YES : MHD_NO;
    }

    return send_response(connection, "{\"error\":\"Invalid Request\"}", MHD_HTTP_BAD_REQUEST);
}

int main() {
    struct MHD_Daemon *server;

    server = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL, &request_handler, NULL, MHD_OPTION_END);
    if (!server) {
        fprintf(stderr, "Failed to start server\n");
        return 1;
    }

    printf("Server running on port %d...\nPress Enter to stop.\n", PORT);
    getchar();
    MHD_stop_daemon(server);
    return 0;
}
