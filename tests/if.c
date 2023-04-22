main: {
  grade := 81u;

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
