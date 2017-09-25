#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <sqlite3.h> 
#include <typeinfo>
#include "tinyxml2.h"

using namespace std;
using namespace tinyxml2;

XMLDocument xmlDoc;
XMLNode * pRoot = xmlDoc.NewElement("itemInformation");

static int callback(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);
   
   for(i = 0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   
   printf("\n");
   return 0;
}

static int createxml(void *data, int argc, char **argv, char **azColName){
   int i;
   fprintf(stderr, "%s: ", (const char*)data);
   XMLElement * pElement = xmlDoc.NewElement("item");
   pElement->SetAttribute(azColName[0], argv[0]);
   pRoot->InsertEndChild(pElement);
   XMLElement * nElement;
   for(i = 1; i<argc; i++){
      //printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      nElement = xmlDoc.NewElement(azColName[i]);
      nElement->SetText(argv[i]);
      pElement->InsertEndChild(nElement);
   }
   
   printf("\n");
   return 0;
}

int main(int argc, char* argv[]) {
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char *sql;
   const char* data = "Callback function called";
   const char* xml = "Create XML Data";

   /* Open database */
   rc = sqlite3_open("data.db", &db);
   
   if( rc ) {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return(0);
   } else {
      fprintf(stderr, "Opened database successfully\n");
   }

   /* Create SQL statement */
   sql = "select id, name, price, description, location, status, note, res_name as responsible_name, item.order_no as order_number, company.c_name as company_name, company.address as company_address from ((item inner join froms on item.order_no = froms.order_no) inner join company on froms.c_name = company.c_name) where item.order_no = ( select max(order_no) from orders );";

   /* Execute SQL statement */
   xmlDoc.InsertFirstChild(pRoot);
   rc = sqlite3_exec(db, sql, createxml, (void*)xml, &zErrMsg);
   
   if( rc != SQLITE_OK ) {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   } else {
      fprintf(stdout, "Operation done successfully\n");
      XMLError eResult = xmlDoc.SaveFile("item.xml");
   }
   sqlite3_close(db);
   return 0;
}
