#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mysql.h>
#include <json.h>
#include <arpa/inet.h>

int 
main ()
{
    char *data;
    MYSQL_RES* result;
    MYSQL_ROW row;
    //struct json_object *jobj;
    char filename[50];
    char buffer1[60];
    char buffer2[1000];
    char *clientString;
    char *dataString;
    struct sockaddr_in client_addr;

    printf ("%s%c%c\n","Content-Type:text/html;charset=iso-8859-1",13,10);
    printf ("<h1>EMBEDDED SYSTEMS - LABO 2</h1><br><br>\n");
    printf ("<form  action=\"/cgi-bin/Labo2\">\n");
    printf ("<label for=\"postData\">Data to submit:</label>\n");
    printf ("<input type=\"text\" id=\"postData\" name=\"postData\" maxlength=\"10\">\n");
    printf ("<input type=\"text\" id=\"clientData\" name=\"clientData\" value=\"\" hidden>\n");
    printf ("<script>document.getElementById('clientData').value= \"%s\";</script>",inet_ntoa(client_addr.sin_addr));
    printf ("<input type=\"submit\" value=\"Submit\">");
    printf ("</form>\n<br>\n<br>\n");

    MYSQL *con = mysql_init(NULL);

    if (con == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        exit(1);
    }

    if (mysql_real_connect(con, "localhost", "liem", "liem123", "labo2", 0, NULL, 0) == NULL)
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        mysql_close(con);
        exit(1);
    }

    data = getenv("QUERY_STRING");

    if(data == NULL)
    {
        printf("<script>console.log(\"No data in querystring.\");</script>");
    }
    else
    {
        dataString = data + 9;
        dataString[strlen(dataString)-20] = '\0';
        clientString = data + 26;
        sprintf(buffer1,"INSERT INTO dataLabo2 (submitData) VALUES ('%s');",dataString);
        sprintf(filename,"%s.json",clientString);
        mysql_query(con, buffer1);
    }   

    mysql_query(con,"SELECT * from dataLabo2;");
    result = mysql_store_result(con);

    FILE *fh = fopen (filename, "w");

    printf("<table><tr><th>Time</th><th>Data</th></tr>");
    fprintf(fh, "[\n");

    while((row = mysql_fetch_row(result)))
    {
        printf("<script>console.log(\"%s : %s\");</script>",row[0],row[1]);
        printf("<tr><th>%s</th><th>%s</th></tr>",row[0],row[1]);
        sprintf(buffer2,"\n{\n\"time\": \"%s\",\n\"data\": \"%s\"\n},",row[0],row[1]);
        fprintf(fh, buffer2);
    }

    printf("</table>");
    printf("<style>table{width: 100%%;}td,th{border: 1px solid #dddddd;text-align: left;padding: 8px;}</style>");
    fprintf(fh, "]\n");

    fclose (fh);

    mysql_close(con);

    return 0;
}