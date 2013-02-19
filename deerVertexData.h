//---------------------------------------------------------------------------
// (compressed) data block for a deer object.
//---------------------------------------------------------------------------
const int NV = 323*3;
float vertices4deer[3*NV]={24,7,11,22,6,9,24,23,12,24,18,15,24,7,11,24,23,12,24,7,14,24,7,11,24,18,15,22,6,9,21,6,10,20,19,12,24,23,12,
  22,6,9,20,19,12,20,18,16,20,7,11,22,7,14,20,19,12,21,6,10,20,7,11,20,19,12,20,7,11,20,18,16,20,19,12,20,18,16,11,18,16,18,20,40,21,7,35,21,7,
  38,18,20,40,22,7,34,21,7,35,22,20,34,22,7,34,18,20,40,21,7,38,25,7,37,24,22,41,18,20,40,21,7,38,24,22,41,25,7,35,24,6,34,22,20,34,25,7,37,25,
  7,35,22,20,34,24,22,41,25,7,37,22,20,34,22,20,34,24,6,34,22,7,34,26,27,39,24,22,41,22,20,34,12,26,45,24,22,41,18,43,47,23,36,15,24,23,12,17,
  32,4,26,27,39,24,24,27,23,32,30,26,27,39,22,20,34,24,24,27,26,27,39,23,32,30,19,43,43,12,26,45,18,20,40,24,22,41,18,43,47,24,22,41,26,27,39,
  18,43,47,26,27,39,19,43,43,27,30,19,24,18,15,24,23,12,27,30,19,24,23,12,23,36,15,12,38,20,23,36,15,14,37,11,19,43,43,23,32,30,15,35,30,19,43,
  43,15,35,30,16,42,41,17,32,4,20,19,12,11,19,11,24,23,12,20,19,12,17,32,4,23,36,15,17,32,4,14,37,11,14,37,11,17,32,4,14,32,5,7,20,14,11,18,16,
  7,27,29,7,27,29,11,18,16,9,20,35,9,20,35,11,18,16,13,20,36,11,18,16,22,20,34,13,20,36,6,30,12,7,20,14,7,27,29,6,30,12,7,27,29,12,38,20,12,38,
  20,7,27,29,15,35,30,7,20,14,7,7,14,11,18,16,11,18,16,7,7,14,10,7,14,7,20,14,9,6,9,7,7,11,7,20,14,7,7,11,7,7,14,11,19,11,10,6,10,9,6,9,11,19,
  11,9,6,9,7,20,14,10,7,14,11,7,12,11,19,11,11,18,16,10,7,14,11,19,11,11,7,12,10,6,10,11,19,11,11,19,11,7,20,14,6,30,12,20,19,12,11,18,16,11,
  19,11,8,20,40,11,7,39,13,20,36,11,7,39,11,7,35,13,20,36,11,7,35,10,7,34,13,20,36,8,20,40,7,7,37,11,7,39,9,20,35,8,6,34,7,7,37,9,20,35,7,7,37,
  8,20,40,13,20,36,10,7,34,9,20,35,10,7,34,8,6,34,9,20,35,8,20,40,13,20,36,18,20,40,13,20,36,22,20,34,18,20,40,12,43,44,12,26,45,18,43,47,9,37,
  42,12,26,45,12,43,44,14,32,5,11,19,11,6,30,12,7,27,29,8,20,40,9,37,42,9,37,42,8,20,40,12,26,45,7,27,29,9,20,35,8,20,40,17,32,4,11,19,11,14,
  32,5,12,26,45,8,20,40,18,20,40,14,37,11,6,30,12,12,38,20,15,35,30,7,27,29,9,37,42,16,42,41,15,35,30,9,37,42,14,32,5,6,30,12,14,37,11,15,58,
  69,15,55,73,19,56,72,14,38,1,13,33,5,16,32,4,20,18,16,22,7,14,24,18,15,12,69,46,16,68,51,14,70,46,14,70,46,16,68,51,18,70,46,12,82,41,12,78,
  41,14,84,38,14,84,38,12,78,41,10,81,37,10,81,37,12,82,41,14,84,38,15,70,43,14,69,41,10,81,37,12,78,41,15,70,43,10,81,37,14,69,41,12,69,41,10,
  81,37,10,81,37,12,69,41,11,69,42,10,81,37,11,69,42,6,86,37,6,86,37,11,69,42,5,83,40,8,81,42,11,69,44,12,69,46,11,69,42,11,69,44,8,81,42,5,83,
  40,11,69,42,8,81,42,12,78,41,14,70,46,15,70,45,12,78,41,12,69,46,14,70,46,8,81,42,12,69,46,12,78,41,8,81,42,12,78,41,12,82,41,12,96,51,8,86,
  42,9,96,44,12,96,51,9,96,44,8,95,52,9,96,44,4,92,44,8,95,52,4,92,44,6,86,37,5,83,40,9,96,44,6,86,37,4,92,44,4,92,44,8,86,42,8,95,52,4,92,44,
  5,83,40,8,86,42,8,95,52,8,86,42,12,96,51,5,83,40,3,83,46,1,90,46,1,90,46,3,83,46,3,89,49,3,83,46,5,83,40,8,81,42,3,83,46,8,81,42,3,89,49,8,
  86,42,5,83,40,1,90,46,1,90,46,3,89,49,8,86,42,3,89,49,8,81,42,8,86,42,12,78,41,15,70,45,15,70,43,14,69,41,11,69,42,12,69,41,8,86,42,10,81,37,
  9,96,44,9,96,44,10,81,37,6,86,37,8,86,42,12,82,41,10,81,37,8,81,42,12,82,41,8,86,42,4,68,47,6,69,41,10,67,42,10,67,42,6,69,41,11,69,42,4,68,
  47,10,67,42,12,69,46,11,69,44,11,69,42,12,69,46,17,57,34,12,43,44,11,57,37,17,57,34,16,42,41,12,43,44,17,57,34,11,57,37,16,66,37,16,66,37,11,
  57,37,10,67,42,16,66,37,10,67,42,11,69,42,14,69,41,16,66,37,11,69,42,11,57,37,12,43,44,5,53,49,11,57,37,5,53,49,10,67,42,10,67,42,5,53,49,12,
  69,46,12,69,46,11,69,42,6,69,41,6,69,41,4,68,47,12,69,46,16,46,51,18,45,57,14,47,58,9,54,66,13,47,65,15,53,71,12,59,55,9,54,66,17,59,70,19,
  60,56,12,59,55,17,59,70,12,59,55,8,53,53,9,54,66,8,53,53,19,47,61,13,47,65,8,53,53,13,47,65,9,54,66,16,68,51,12,59,55,19,60,56,12,69,46,12,
  59,55,16,68,51,9,54,66,15,53,71,17,59,70,12,59,55,5,53,49,8,53,53,12,69,46,5,53,49,12,59,55,5,53,49,14,47,58,8,53,53,12,43,44,18,43,47,16,46,
  51,5,53,49,12,43,44,16,46,51,5,53,49,16,46,51,14,47,58,16,42,41,9,37,42,12,43,44,24,24,27,20,18,16,24,18,15,22,20,34,11,18,16,20,18,16,22,20,
  34,20,18,16,24,24,27,24,24,27,24,18,15,27,30,19,23,32,30,24,24,27,27,30,19,23,32,30,27,30,19,23,36,15,15,35,30,23,32,30,23,36,15,12,38,20,15,
  35,30,23,36,15,22,7,14,24,7,14,24,18,15,15,70,45,14,70,46,18,70,46,20,82,40,20,78,41,24,81,42,23,96,44,24,86,42,20,96,51,24,95,52,23,96,44,
  20,96,51,24,95,52,28,92,44,23,96,44,27,83,40,26,86,37,28,92,44,28,92,44,26,86,37,23,96,44,24,95,52,24,86,42,28,92,44,24,86,42,27,83,40,28,92,
  44,20,96,51,24,86,42,24,95,52,31,90,46,29,83,46,27,83,40,29,89,49,29,83,46,31,90,46,24,81,42,27,83,40,29,83,46,29,89,49,24,81,42,29,83,46,31,
  90,46,27,83,40,24,86,42,24,86,42,29,89,49,31,90,46,24,86,42,24,81,42,29,89,49,20,78,41,17,70,42,17,70,44,17,70,42,18,69,41,17,70,44,20,69,41,
  21,69,42,18,69,41,15,70,45,18,70,46,17,70,44,23,96,44,22,81,37,24,86,42,22,81,37,20,82,40,24,86,42,26,86,37,22,81,37,23,96,44,24,86,42,20,82,
  40,24,81,42,22,67,42,26,69,41,28,68,46,21,69,42,26,69,41,22,67,42,20,69,45,22,67,42,28,68,46,20,69,45,21,69,42,20,69,44,19,43,43,16,42,41,17,
  57,34,27,53,48,19,43,43,17,57,34,22,67,42,17,57,34,16,66,37,21,69,42,16,66,37,18,69,41,21,69,42,22,67,42,16,66,37,22,67,42,27,53,48,17,57,34,
  20,69,45,27,53,48,22,67,42,26,69,41,21,69,42,20,69,45,20,69,45,28,68,46,26,69,41,24,53,53,14,47,58,18,45,57,8,53,53,14,47,58,24,53,53,18,45,
  57,16,46,51,24,53,53,15,53,71,13,47,65,24,54,68,24,54,68,13,47,65,19,47,61,17,59,70,24,54,68,19,60,56,24,54,68,24,53,53,19,60,56,24,54,68,19,
  47,61,24,53,53,16,68,51,19,60,56,20,69,45,15,53,71,24,54,68,17,59,70,24,53,53,27,53,48,19,60,56,19,60,56,27,53,48,20,69,45,24,53,53,19,47,61,
  8,53,53,16,46,51,18,43,47,27,53,48,24,53,53,16,46,51,27,53,48,27,53,48,18,43,47,19,43,43,20,78,41,20,69,45,24,81,42,11,7,12,14,1,17,10,6,10,
  14,1,17,12,1,7,10,6,10,6,1,22,14,1,17,11,5,19,10,7,14,11,5,19,11,7,12,10,6,10,12,1,7,9,6,9,7,7,14,6,1,22,11,5,19,9,7,15,7,7,14,11,5,19,9,7,
  15,11,5,19,10,7,14,12,1,7,5,1,6,9,6,9,14,1,17,5,1,6,12,1,7,6,1,22,5,1,6,14,1,17,7,6,9,7,7,14,7,7,11,9,6,9,5,1,6,7,6,9,5,1,6,6,1,22,7,7,14,7,
  6,9,5,1,6,7,7,14,19,56,72,15,55,73,15,57,69,17,70,44,18,69,41,15,70,43,15,70,43,15,70,45,17,70,44,18,69,41,16,66,37,15,70,43,15,70,43,16,66,
  37,14,69,41,18,70,46,16,68,51,20,69,45,17,84,38,20,78,41,20,82,40,22,81,37,20,78,41,17,84,38,17,84,38,20,82,40,22,81,37,22,81,37,18,69,41,17,
  70,42,22,81,37,17,70,42,20,78,41,22,81,37,20,69,41,18,69,41,21,69,42,20,69,41,22,81,37,26,86,37,21,69,42,22,81,37,27,83,40,21,69,42,26,86,37,
  20,69,45,20,69,44,24,81,42,24,81,42,20,69,44,21,69,42,24,81,42,21,69,42,27,83,40,17,70,44,18,70,46,20,78,41,18,70,46,20,69,45,20,78,41,11,5,
  19,14,1,17,11,7,12,24,7,14,25,1,22,26,1,6,24,7,14,26,1,6,24,6,9,11,7,39,7,7,37,10,7,39,10,7,39,7,7,37,8,7,39,11,7,37,11,7,35,11,7,39,12,2,46,
  13,2,31,11,7,37,11,7,39,12,2,46,11,7,37,11,7,37,13,2,31,11,7,35,8,7,39,5,5,43,10,7,39,5,5,43,12,2,46,10,7,39,10,7,39,12,2,46,11,7,39,10,7,34,
  13,2,31,8,6,34,11,7,35,13,2,31,10,7,34,13,2,31,7,1,30,8,6,34,7,1,30,13,2,31,12,2,46,7,1,30,12,2,46,3,1,41,5,5,43,3,1,41,12,2,46,7,7,37,5,5,
  43,8,7,39,8,6,34,5,5,43,7,7,37,8,6,34,3,1,41,5,5,43,8,6,34,7,1,30,3,1,41,10,7,14,7,7,14,9,7,15,9,6,9,7,6,9,7,7,11,24,6,9,26,1,6,22,6,9,22,7,
  14,20,7,11,20,7,13,24,7,11,24,6,9,22,6,9,20,7,13,18,1,17,20,5,19,21,6,10,18,1,7,18,1,17,21,6,10,18,1,17,20,7,13,20,5,19,18,1,17,25,1,22,20,
  7,11,21,6,10,20,7,13,22,6,9,18,1,7,21,6,10,20,5,19,25,1,22,24,7,14,20,5,19,24,7,14,22,7,14,20,7,13,20,5,19,22,7,14,22,6,9,26,1,6,18,1,7,18,1,
  7,26,1,6,18,1,17,18,1,17,26,1,6,25,1,22,24,7,11,24,7,14,24,6,9,29,1,41,25,1,30,24,6,34,24,6,34,19,2,31,22,7,34,22,7,34,19,2,31,21,7,35,24,6,
  34,25,1,30,19,2,31,20,2,46,19,2,31,25,1,30,29,1,41,20,2,46,25,1,30,20,2,46,29,1,41,28,5,42,24,7,39,28,5,42,25,7,37,25,7,35,28,5,42,24,6,34,
  25,7,37,28,5,42,25,7,35,28,5,42,29,1,41,24,6,34,21,7,38,20,2,46,22,7,39,21,7,38,21,7,35,21,7,37,21,7,37,19,2,31,20,2,46,21,7,37,20,2,46,21,7,
  38,21,7,35,19,2,31,21,7,37,22,7,39,20,2,46,28,5,42,22,7,39,28,5,42,24,7,39,24,7,39,25,7,37,22,7,39,12,62,54,13,61,55,15,61,56,18,67,52,21,65,
  51,20,68,50,13,69,50,12,65,51,15,67,51,22,7,39,25,7,37,21,7,38,12,62,54,15,61,56,14,67,51,18,65,53,19,62,55,21,63,53,19,66,50,21,61,53,21,64,
  51,17,63,55,19,60,56,19,66,50,19,66,50,19,60,56,21,61,53,19,60,56,21,60,54,21,61,53,12,64,53,12,63,55,15,65,53,12,63,55,15,62,55,15,65,53,12,
  65,52,12,62,54,14,67,51,14,67,51,15,61,56,17,63,54,18,65,53,21,63,53,20,64,52};
//---------------------------------------------------------------------------
float colors4deer[4*NV]={1,7,3,4,1,3,6,1,4,7,0,5,3,7,5,2,4,0,5,0,4,5,4,2,5,2,2,3,0,3,3,5,0,9,5,3,3,9,0,3,3,0,8,1,9,9,8,9,0,9,9,9,1,5,1,
  0,9,4,0,6,0,3,2,1,4,2,1,9,4,1,2,2,4,3,0,6,0,1,6,1,2,0,4,3,0,3,0,7,0,4,2,2,0,2,0,10,2,5,9,3,5,2,0,2,4,4,2,4,4,2,6,6,2,7,7,2,1,2,9,1,3,4,6,3,6,
  7,7,6,0,4,5,2,2,5,8,4,0,6,4,6,5,9,4,0,9,0,4,8,6,9,2,8,9,6,4,9,9,4,3,5,2,9,3,4,1,4,7,1,7,9,1,3,4,4,7,2,4,7,4,3,1,9,7,9,2,7,3,1,3,1,9,3,2,4,6,
  3,4,2,4,9,3,6,3,3,3,3,4,6,7,3,2,9,4,4,3,3,4,3,7,0,6,3,10,0,3,4,3,4,5,4,5,8,10,1,2,0,4,4,8,5,5,8,0,2,2,2,2,2,3,3,2,2,6,1,3,2,6,3,1,3,3,3,3,3,
  3,3,1,1,3,3,7,8,4,3,8,7,3,3,7,2,5,3,2,4,5,7,4,2,7,2,2,9,4,10,9,10,1,10,5,1,5,1,3,10,1,5,5,4,1,5,3,4,1,4,9,3,10,0,0,10,2,10,3,7,10,7,2,4,3,0,
  0,2,4,2,7,4,2,3,6,1,3,3,4,3,10,10,3,1,4,2,3,7,2,4,6,7,5,5,7,3,6,5,4,8,3,4,1,2,0,1,10,2,1,0,1,1,0,5,1,5,3,1,1,3,0,2,6,0,6,5,5,6,4,4,3,7,7,6,4,
  1,0,0,8,1,9,5,8,1,5,5,1,5,7,8,7,1,1,7,1,8,8,5,5,4,5,8,8,9,1,5,6,7,4,6,5,6,0,7,2,6,1,6,2,1,6,1,0,10,3,2,4,2,4,9,2,2,9,2,4,4,4,0,2,4,0,2,0,0,0,
  2,0,7,0,0,0,6,4,3,5,0,3,3,4,9,4,5,0,9,5,0,9,9,8,3,9,9,3,9,0,4,9,4,8,9,5,4,0,5,6,8,7,6,5,4,8,6,7,4,6,5,8,4,4,7,5,4,4,7,3,6,5,6,1,5,2,2,1,3,0,
  5,9,2,4,2,3,4,3,2,9,4,3,4,8,1,3,2,1,8,5,8,3,5,2,1,2,10,1,2,2,1,8,1,1,2,1,1,2,8,1,8,2,1,5,2,8,1,2,5,5,3,1,2,0,0,7,0,2,0,1,2,9,1,0,0,1,1,1,0,5,
  0,2,5,0,1,2,8,5,5,9,0,1,2,2,5,5,2,5,2,1,7,1,6,2,2,1,2,2,6,2,3,5,4,6,10,4,10,7,4,1,10,0,8,0,6,4,7,0,5,1,0,5,5,0,5,0,8,7,5,0,10,5,7,1,5,10,5,5,
  6,0,5,5,5,1,5,5,5,5,0,9,1,5,1,6,6,3,5,1,1,6,6,1,1,0,8,5,7,3,3,2,3,7,7,3,2,2,1,6,2,6,3,2,2,1,2,2,2,3,2,2,8,2,3,5,1,4,4,1,2,4,2,8,5,0,3,0,5,3,
  0,10,6,6,6,10,6,10,2,4,4,7,7,4,5,2,7,4,4,0,2,4,4,2,2,0,7,5,4,7,4,4,7,7,4,4,9,0,2,7,0,9,0,9,2,9,0,4,9,4,9,4,9,4,4,4,5,2,4,4,2,9,4,2,9,9,8,5,5,
  0,5,6,2,10,7,0,4,7,1,2,7,7,0,7,0,8,0,0,0,7,7,0,6,4,0,7,7,8,0,7,6,6,7,6,0,7,7,0,7,10,8,8,10,0,0,10,6,1,6,2,4,8,1,1,4,5,5,4,0,1,8,4,3,4,8,4,3,
  8,3,4,1,7,1,9,8,1,1,9,1,8,1,4,1,3,3,9,3,0,1,1,4,3,1,3,3,0,4,1,9,3,1,9,1,7,7,9,9,2,1,0,2,2,9,4,9,2,9,9,3,2,0,6,8,3,8,8,6,2,8,9,8,8,9,6,9,1,6,
  3,8,0,8,0,0,8,2,6,6,0,4,8,8,0};
//---------------------------------------------------------------------------