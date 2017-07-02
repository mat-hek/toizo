inline static void brut_changePos(int where, int* i, int* j) {
  switch (where) {
    case 0: (*j)++; break;
    case 1: (*i)++; break;
    case 2: (*j)--; break;
    case 3: (*i)--; break;
  }
}

inline static void brut_move(int where, int* i, int* j, char A[N][N]) {
  while(1) {
    brut_changePos(where, i, j);
    switch(A[*i][*j]){
      case '%':
      case '|': continue;
      case '\\': where ^= 1; continue;
      case '/': where ^= 3; continue;
    }
    break;
  }
}

char brut_solve_r(char A[N][N], int i, int j, int dir, int lcnt, int ccnt);

inline static char brut_mirror45(char A[N][N], int i, int j, int dir, int lcnt, int ccnt) {
  A[i][j] = '/';
  return brut_solve_r(A, i, j, dir^3, lcnt-1, ccnt);
}

inline static char brut_mirror135(char A[N][N], int i, int j, int dir, int lcnt, int ccnt) {
  A[i][j] = '\\';
  return brut_solve_r(A, i, j, dir^1, lcnt-1, ccnt);
}

inline static char brut_mirror90(char A[N][N], int i, int j, int dir, int lcnt, int ccnt) {
  A[i][j] = '|';
  if(brut_solve_r(A, i, j, dir, lcnt, ccnt)) {
    A[i][j] = ' ';
    return 1;
  } else {
    return 0;
  }
}

char brut_solve_r(char A[N][N], int i, int j, int dir, int lcnt, int ccnt) {
  brut_move(dir, &i, &j, A);
  char r;
  switch(A[i][j]) {
    case ' ':
      if(
        brut_mirror90(A, i, j, dir, lcnt, ccnt)
        || (lcnt > 0 && (
          rand()%2 ?
            brut_mirror45(A, i, j, dir, lcnt, ccnt) || brut_mirror135(A, i, j, dir, lcnt, ccnt)
          :
            brut_mirror135(A, i, j, dir, lcnt, ccnt) || brut_mirror45(A, i, j, dir, lcnt, ccnt)
        ))
      ) {
        return 1;
      } else {
        A[i][j] = ' ';
        return 0;
      }
    case '*':
      if(ccnt == 1) return 1;
      A[i][j] = '%';
      r = brut_solve_r(A, i, j, dir, lcnt, ccnt-1);
      A[i][j] = '*';
      return r;
    default:
      return 0;
  }
}

void brut_solve(char A[N][N], int w, int h, int lcnt) {
  int ccnt = 0;
  for(int i=1; i <= h; i++)
    for(int j=1; j <= w; j++)
      if (A[i][j] == '*') ccnt++;

  for(int i=lcnt; i <= lcnt && !brut_solve_r(A, 2, 0, 0, i, ccnt); i++);

}
