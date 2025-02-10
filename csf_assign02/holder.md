int getRow(int index, int width){
  return index/width; //index/width + 1
}

int getCol(int index, int width){
  int row = getRow(index, width);
  return index - (row * width); //index - ((row - 1) * width) + 1
}

uint64_t getRowTransform(int index, int height, int width){
  return 1000000 - (2000000000 * getRow(index, width) / (1000000 * width));
}

uint64_t getColumnTransform(int index, int height, int width){
  return 1000000 - (2000000000 * getCol(index, width) / (1000000 * height));
}

uint64_t getModifiedComponentValue(int index, int height, int width, uint8_t color){
  return color * getRowTransform(index, height, width) * getColumnTransform(index, height, width);
}