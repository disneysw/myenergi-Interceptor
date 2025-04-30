#include "include.h"
#include "file.h"

// ---------------------------------------------------
String readFile(fs::FS &fs, const char *path)
{
  Serial.printf("Reading file: %s\r\n", path);
  File file = fs.open(path, "r");
  if (!file || file.isDirectory())
  {
    Serial.println("- empty file or failed to open file");
    return String();
  }
  Serial.println("- read from file:");
  String fileContent;
  while (file.available())
  {
    fileContent += String((char)file.read());
  }
  file.close();
  Serial.println(fileContent);
  return fileContent;
}

// ---------------------------------------------------
void writeFile(fs::FS &fs, const char *path, const char *message)
{
  Serial.printf("Writing file: %s\r\n", path);
  File file = fs.open(path, "w");
  if (!file)
  {
    Serial.println("- failed to open file for writing");
    return;
  }
  if (file.print(message))
  {
    Serial.println("- file written");
  }
  else
  {
    Serial.println("- write failed");
  }
  file.close();
}

//---------------------------------------------------
bool InitFilesystem()
{
  // Initialize LittleFS
  if (!LittleFS.begin(false /* false: Do not format if mount failed */))
  {
    Serial.println("Failed to mount LittleFS");
    if (!LittleFS.begin(true /* true: format */))
    {
      Serial.println("Failed to format LittleFS");
    }
    else
    {
      Serial.println("LittleFS formatted successfully");
      return true;
    }
  }
  else
  { // Initial mount success
    Serial.println("LittleFS mounted successfully");
    return true;
  }
  Serial.println("LittleFS format failed");
  return false;
}

