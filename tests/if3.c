int main() {
  int grade = 20;

  if (grade > 80) {
    return 100;
  } else if (grade > 60) {
    return 80;
  } else if (grade > 40) {
    return 60;
  } else {
    return 40;
  }
}
