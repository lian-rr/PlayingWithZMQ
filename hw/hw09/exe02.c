#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *json;

struct professor
{
    long long id;
    char *name;
    char *degree;
    char *email;
    long long phone;
};

int callback(void *, int, char **, char **);

void build_json(struct professor *);

int save_json();

char *
string_concat(char *str1, char *str2)
{
    char *result = malloc(strlen(str1) + strlen(str2) + 1);
    strcpy(result, str1);
    strcat(result, str2);
    return strdup(result);
}

char *trim_string(const char *str)
{
    char *aux = strdup(str);
    aux[strlen(aux) - 1] = 0;
    return strdup(aux);
}

int main(void)
{

    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open("professors.db", &db);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n",
                sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    json = malloc(sizeof(char) * 10000);

    sprintf(json, "{\n\"professors\":[\n");

    char *sql = "SELECT * FROM Professors";
    rc = sqlite3_exec(db, sql, callback, 0, &err_msg);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Failed to select data\n");
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    json = trim_string(json);
    json = string_concat(json, "]\n}\n");
    printf("%s", json);

    save_json(json);

    sqlite3_close(db);
    return 0;
}

int callback(void *NotUsed, int argc, char **argv,
             char **azColName)
{
    NotUsed = 0;

    struct professor *p = malloc(sizeof(struct professor));

    int i;
    for (i = 0; i < argc; i++)
    {
        if (strcasecmp(azColName[i], "id") == 0)
            p->id = (long long)atoi(argv[i]);
        if (strcasecmp(azColName[i], "name") == 0)
            p->name = strdup(argv[i]);
        if (strcasecmp(azColName[i], "degree") == 0)
            p->degree = strdup(argv[i]);
        if (strcasecmp(azColName[i], "email") == 0)
            p->email = strdup(argv[i]);
        if (strcasecmp(azColName[i], "phone") == 0)
            p->phone = (long long)atoi(argv[i]);
    }

    build_json(p);

    return 0;
}

void build_json(struct professor *p)
{
    char *buf = malloc(sizeof(char) * 200);

    sprintf(buf, "{\"id\": %lld,\"name\": \"%s\",\"degree\": \"%s\",\"email\": \"%s\",\"phone\": %lld},",
            p->id, p->name, p->degree, p->email, p->phone);

    json = string_concat(json, buf);

    free(buf);
}

int save_json()
{
    FILE *df;

    df = fopen("./output.json", "w");

    if (df == NULL)
        return -1;

    fprintf(df, "%s", json);

    fclose(df);
    return 0;
}