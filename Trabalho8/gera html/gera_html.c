#include <stdio.h>

#define AUTOMATICO 0
#define MANUAL 1

void cria_arquivo(char buffer[]){
    FILE *fp;

    fp = fopen ("index_gerado.html", "w"); 
    int i;
    for (i = 0; buffer[i] != '\n'; i++) {
        /* write to file using fputc() function */
        fputc(buffer[i], fp);
    }
    fclose(fp);
}

void gera_html(char html[10000], int modo, int intensidade, int luminosidade){
    int n;
    n = sprintf(html,
    "<!DOCTYPE html>\
    \
    <html>\
    \
    <head>\
        <meta http-equiv=\"refresh\" content=\"1\" />\
    </head>\
    \
    <body>\
        <h1 style=\"text-align: center\">Trabalho 8 - Sistema Lumiar</h1><br>\
    \
        <center><img src=\"img/Poli-minerva-cortado.png\" alt=\"Logo Poli\" width=\"250pxpx\" height=\"auto\"></center><br>\
        <h2 style=\"margin-left: 0px\" align=\"center\">LUMIAR</h2><br>\
    \
    \
    \
        <table style=\"width:50%;border: 0px solid black\" align=\"center\">\
            <tr>\
                <td>\
                    <h3>Estado</h3>\
                </td>\
    \
            </tr>\
            <tr>\
                <td>\
                    <form action=\"selec_modo\" method=\"GET\">\
                        <table style=\"width:100%\">\
                            <tr>\
                                <td>\
                                    Modo de operação:\
                                </td>\
                                <td>\
                                    <select id=\"modo\" name=\"modo\">\
                                        <option value=\"MANUAL\">MANUAL</option>\
                                        <option value=\"AUTO\">AUTOMATICO</option>\
                                    </select><br>\
                                </td>\
                            </tr>\
                            <tr>\
                                <td>\
                                    Intensidade do LED:\
                                </td>\
                                <td>\
                                    <input type=\"text\" name=\"intensidade\" value=\"%d\"><br>\
                                </td>\
                            </tr>\
                            <tr>\
                                <td>\
                                    Luminosidade:\
                                </td>\
                                <td>\
                                    <input type=\"text\" name=\"luminosidade\" value=\"%d\"><br>\
                                </td>\
                            </tr>\
                        </table>\
                        <br><input type=\"submit\" value=\"Submit\">\
                    </form>\
                </td>\
                <td>\
                    <center><img src=\"img/lampada2.png\" alt=\"Logo Poli\" width=\"250pxpx\" height=\"auto\"\
                            style=\"background-color: hsl(60, 100%%, %d%%)\">\
                    </center>\
                </td>\
            </tr>\
        </table>\
    </body>\
    </html>\n", intensidade, luminosidade, (200 - intensidade)/2
    );
    cria_arquivo(html);
}


int main(){
    char html[10000];
    gera_html(html, MANUAL,50,30);
    return 0;
}