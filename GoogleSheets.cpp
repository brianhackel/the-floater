#include "GoogleSheets.h"

const String GoogleSheets::NAME = "GoogleSheets";
const String GoogleSheets::BASE_URL = "https://script.google.com/macros/s/";

GoogleSheets::GoogleSheets(const String& deploymentId, const String& spreadsheetId, const String& sheetName) : 
  Poster(BASE_URL + deploymentId + "/exec", NAME), 
  _spreadsheetId(spreadsheetId), 
  _sheetName(sheetName) {};

String GoogleSheets::getJson(float angle, float temperature, long battery) const {
  String jsonString = "";
  jsonString += "{\"temp\":\"";
  jsonString += temperature;
  jsonString += "\",\"tilt\":\"";
  jsonString += angle;
  jsonString += "\",\"battery\":\"";
  jsonString += battery;
  jsonString += "\",\"spreadsheetId\":\"";
  jsonString += _spreadsheetId;
  jsonString += "\",\"sheetName\":\"";
  jsonString += _sheetName;
  jsonString += "\"}";
  return jsonString;
}

