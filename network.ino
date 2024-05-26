/*
  Network driver

  -> Suspeded for now
*/

String getNet(String data){
  esp.println(data);
  String input = esp.readStringUntil('\n');
  
  return input;
}