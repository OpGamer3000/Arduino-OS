/*
  Suspeded for now
*/

String getNet(String data){
  esp.print(data);
  String input = esp.readStringUntil('\n');
  
  return input;
}