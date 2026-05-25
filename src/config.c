#include "config.h"

#include <stdlib.h>
#include <string.h>

struct config {
    char *input_dir;
    char *output_dir;
    char *geo_file;
    char *qry_file;
    char *via_file;
};

static char *copy_text(const char *text) {
    if (text == NULL) {
        return NULL;
    }

    size_t length = strlen(text) + 1;
    char *copy = (char *)malloc(length);
    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy, text, length);
    return copy;
}

static void release_fields(struct config *config) {
    if (config == NULL) {
        return;
    }

    free(config->input_dir);
    free(config->output_dir);
    free(config->geo_file);
    free(config->qry_file);
    free(config->via_file);

    config->input_dir = NULL;
    config->output_dir = NULL;
    config->geo_file = NULL;
    config->qry_file = NULL;
    config->via_file = NULL;
}

static bool set_text(char **field, const char *value) {
    char *copy = copy_text(value);
    if (copy == NULL) {
        return false;
    }

    free(*field);
    *field = copy;
    return true;
}

static bool reset_to_defaults(struct config *config) {
    release_fields(config);
    return set_text(&config->input_dir, ".");
}

Config config_create(void) {
    struct config *config = (struct config *)calloc(1, sizeof(struct config));
    if (config == NULL) {
        return NULL;
    }

    if (!reset_to_defaults(config)) {
        free(config);
        return NULL;
    }

    return config;
}

bool config_parse_args(Config config_ref, int argc, char *argv[]) {
    struct config *config = (struct config *)config_ref;
    if (config == NULL || argv == NULL || !reset_to_defaults(config)) {
        return false;
    }

    for (int i = 1; i < argc; i++) {
        char **target = NULL;

        if (strcmp(argv[i], "-e") == 0) {
            target = &config->input_dir;
        } else if (strcmp(argv[i], "-f") == 0) {
            target = &config->geo_file;
        } else if (strcmp(argv[i], "-q") == 0) {
            target = &config->qry_file;
        } else if (strcmp(argv[i], "-v") == 0) {
            target = &config->via_file;
        } else if (strcmp(argv[i], "-o") == 0) {
            target = &config->output_dir;
        } else {
            return false;
        }

        if (i + 1 >= argc || argv[i + 1][0] == '-') {
            return false;
        }

        if (!set_text(target, argv[i + 1])) {
            return false;
        }
        i++;
    }

    return config->geo_file != NULL && config->output_dir != NULL;
}

const char *config_get_input_dir(Config config_ref) {
    struct config *config = (struct config *)config_ref;
    return config != NULL ? config->input_dir : NULL;
}

const char *config_get_output_dir(Config config_ref) {
    struct config *config = (struct config *)config_ref;
    return config != NULL ? config->output_dir : NULL;
}

const char *config_get_geo_file(Config config_ref) {
    struct config *config = (struct config *)config_ref;
    return config != NULL ? config->geo_file : NULL;
}

const char *config_get_qry_file(Config config_ref) {
    struct config *config = (struct config *)config_ref;
    return config != NULL ? config->qry_file : NULL;
}

const char *config_get_via_file(Config config_ref) {
    struct config *config = (struct config *)config_ref;
    return config != NULL ? config->via_file : NULL;
}

void config_destroy(Config config_ref) {
    struct config *config = (struct config *)config_ref;
    if (config == NULL) {
        return;
    }

    release_fields(config);
    free(config);
}
