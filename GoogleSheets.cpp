#include "GoogleSheets.h"

const String GoogleSheets::NAME = "GoogleSheets";
const String GoogleSheets::BASE_URL = "https://script.google.com/macros/s/";

GoogleSheets::GoogleSheets(const String& deploymentId, const String& spreadsheetId, const String& sheetName) : Poster(BASE_URL + deploymentId + "/exec", NAME), _spreadsheetId(spreadsheetId), _sheetName(sheetName) {};

/*
{"temp":"77",
"tilt":"44",
"battery":"88",
"sheetName":"Log",
"spreadsheetId":"1Vm_yyzYJGHxuL6VfqA0Vs1Op3U4c2ru-9uVMdvuiXVs"} 

https://script.google.com/macros/s/AKfycbwaDRFxaTrB77QBs42C7wUi7NCoxxAsYd1n5g4Cpo6R-oWpYTpNfPOOwiyXs9RCNHgp/exec
*/


String GoogleSheets::getJson(float angle, float temperature, long battery) const {
  String jsonString = "";
  jsonString += "{\"temp\":\"";
  jsonString += temperature;
  jsonString += "\",\"tilt\":\"";
  jsonString += angle;
  jsonString += "\",\"battery\":\"";
  jsonString += battery;
  jsonString += "\"}";
  return jsonString;
}

