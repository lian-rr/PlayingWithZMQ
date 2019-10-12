#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <nxjson.h>

struct professor
{
    long long id;
    char *name;
    char *degree;
    char *email;
    long long phone;
};

char *
string_concat(char *str1, char *str2)
{
    char *result = malloc(strlen(str1) + strlen(str2) + 1);
    strcpy(result, str1);
    strcat(result, str2);
    return strdup(result);
}

int main(void)
{

    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open("professors.db", &db);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return 1;
    }

    char *sql = "DROP TABLE IF EXISTS professors;"
                "CREATE TABLE professors(id INT, name TEXT, degree TEXT, email TEXT, phone INT);";

    FILE *f = fopen("data/data.json", "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET); /* same as rewind(f); */

    char *input = malloc(fsize + 1);
    fread(input, 1, fsize, f);
    fclose(f);

    input[fsize] = 0;

    const nx_json *json = nx_json_parse_utf8(input);

    const nx_json *professors = nx_json_get(json, "professors");

    nx_json *prof = professors->child;
    for (prof = professors->child; prof; prof = prof->next)
    {
        struct professor *p = malloc(sizeof(struct professor));

        nx_json *field = prof->child;
        for (field = prof->child; field; field = field->next)
        {
            if (field->type == NX_JSON_STRING)
            {
                if (strcasecmp(field->key, "name") == 0)
                    p->name = strdup(field->text_value);
                if (strcasecmp(field->key, "degree") == 0)
                    p->degree = strdup(field->text_value);
                if (strcasecmp(field->key, "email") == 0)
                    p->email = strdup(field->text_value);
            }
            else if (field->type == NX_JSON_INTEGER)
            {
                if (strcasecmp(field->key, "id") == 0)
                    p->id = field->int_value;
                if (strcasecmp(field->key, "phone") == 0)
                    p->phone = field->int_value;
            }
        }
        char *buf = malloc(sizeof(char) * 200);
        sprintf(buf, "INSERT INTO professors VALUES(%lld, '%s', '%s', '%s', '%lld');",
                p->id, p->name, p->degree, p->email, p->phone);
        
        sql = string_concat(sql, buf);
    }

    rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK)
    {

        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return 1;
    }

    sqlite3_close(db);

    return 0;
}