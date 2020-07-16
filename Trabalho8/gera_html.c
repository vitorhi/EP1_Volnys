#include <stdio.h>

#define AUTOMATICO 0
#define MANUAL 1
#define REFRESHTIME 1

void cria_arquivo(char buffer[], char dir_path[]) {
  FILE *fp;
  char caminho[200];
  strcpy(caminho, dir_path);
  fp = fopen(strcat(caminho, "/index.html"), "w");
  int i;
  for (i = 0; buffer[i] != '\n'; i++) {
    /* write to file using fputc() function */
    fputc(buffer[i], fp);
  }
  fclose(fp);
}

void gera_html(char html[10000], char dir_path[], int modo, int intensidade,
               int luminosidade) {
  int n;
  if (modo == AUTOMATICO) {
    intensidade = 100 - luminosidade;
    n = sprintf(html, "<!DOCTYPE html>\
    \
    <html>\
    \
    <head>\
        <meta http-equiv=\"refresh\" content=\"%d\" />\
    </head>\
    \
    <body>\
        <h1 style=\"text-align: center\">Trabalho 8 - Sistema Lumiar</h1><br>\
    \
        <center><img src=\"/img/Poli-minerva-cortado.png\" alt=\"Logo Poli\" width=\"250pxpx\" height=\"auto\"></center><br>\
        <h2 style=\"margin-left: 0px\" align=\"center\">LUMIAR</h2><br>\
    \
    \
    \
        <table style=\"width:50%%;border: 0px solid black\" align=\"center\">\
            <tr>\
                <td>\
                    <h3>Estado</h3>\
                </td>\
    \
            </tr>\
            <tr>\
                <td>\
                    <form action=\"selec_modo\" method=\"GET\">\
                        <table style=\"width:100%%\">\
                            <tr>\
                                <td>\
                                    Modo de operação:\
                                </td>\
                                <td>\
                                    AUTOMATICO\
				                    <button name=\"selec_modo\" type=\"submit\" value=\"MANUAL\">Mudar para Manual</button>\
                                </td>\
                            </tr>\
                            <tr>\
                                <td>\
                                    Intensidade do LED:\
                                </td>\
                                <td>\
                                    %d\
                                </td>\
                            </tr>\
                            <tr>\
                                <td>\
                                    Luminosidade:\
                                </td>\
                                <td>\
                                    %d\
                                </td>\
                            </tr>\
                        </table>\
                    </form>\
                </td>\
                <td>\
                    <center><img src=\"/img/lampada2.png\" alt=\"Logo Poli\" width=\"250pxpx\" height=\"auto\"\
                            style=\"background-color: hsl(60, 100%%, %d%%)\">\
                    </center>\
                </td>\
            </tr>\
        </table>\
    </body>\
    </html>\n",
                REFRESHTIME, intensidade, luminosidade,
                (200 - intensidade) / 2);
  } else if (modo == MANUAL) {
    n = sprintf(html, "<!DOCTYPE html>\
    \
    <html>\
    \
    <head>\
    </head>\
    \
    <body>\
        <h1 style=\"text-align: center\">Trabalho 8 - Sistema Lumiar</h1><br>\
    \
        <center><img src=\"/img/Poli-minerva-cortado.png\" alt=\"Logo Poli\" width=\"250pxpx\" height=\"auto\"></center><br>\
        <h2 style=\"margin-left: 0px\" align=\"center\">LUMIAR</h2><br>\
    \
    \
    \
        <table style=\"width:50%%;border: 0px solid black\" align=\"center\">\
            <tr>\
                <td>\
                    <h3>Estado</h3>\
                </td>\
    \
            </tr>\
            <tr>\
                <td>\
                        <table style=\"width:100%%\">\
                            <tr>\
                                <td>\
                                    Modo de operação:\
                                </td>\
                                <td>\
                                    <form action=\"selec_modo\" method=\"GET\">\
                                        MANUAL\
				                    <button name=\"selec_modo\" type=\"submit\" value=\"AUTOMATICO\">Mudar para Automatico</button>\
                                    </form>\
                                </td>\
                            </tr>\
                            <tr>\
                                <td>\
                                    Intensidade do LED:\
                                </td>\
                                <td>\
                                    <form action=\"parametros\" method=\"GET\">\
                                        <input type=\"text\" name=\"intensidade\" value=\"%d\"><input type=\"submit\" value=\"Enviar\">\
                                    </form>\
                                </td>\
                            </tr>\
                            <tr>\
                                <td>\
                                    Luminosidade:\
                                </td>\
                                <td>\
                                    %d\
                                </td>\
                            </tr>\
                        </table>\
                </td>\
                <td>\
                    <center><img src=\"/img/lampada2.png\" alt=\"Logo Poli\" width=\"250pxpx\" height=\"auto\"\
                            style=\"background-color: hsl(60, 100%%, %d%%)\">\
                    </center>\
                </td>\
            </tr>\
        </table>\
    </body>\
    </html>\n", intensidade, luminosidade,
                (200 - intensidade) / 2);
  }
  cria_arquivo(html, dir_path);
}