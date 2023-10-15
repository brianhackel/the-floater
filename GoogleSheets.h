#ifndef _GOOGLE_SHEETS_H
#define _GOOGLE_SHEETS_H
#include "Poster.h"

class GoogleSheets : public Poster {
  private:
    static const String BASE_URL;
    static const String NAME;

  protected:
    const String _spreadsheetId;
    const String _sheetName;

  public:
    GoogleSheets(const String& deploymentId, const String& spreadsheetId, const String& sheetName);
    String getJson(float angle, float temperature, long batteryPercentage) const override;
};

#endif
