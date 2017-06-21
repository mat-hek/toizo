#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define N 102
#define E 'x'

// void printBoard(char A[N][N], int w, int h) {
//   for(int i=1; i<=h; i++) {
//     for(int j=1; j<=w; j++)
//       printf("%c", A[i][j]);
//     printf("\n");
//   }
// }

inline static void changePos(int where, int* i, int* j) {
  switch (where) {
    case 0: (*j)++; break;
    case 1: (*i)++; break;
    case 2: (*j)--; break;
    case 3: (*i)--; break;
  }
}

inline static void move(int where, int* i, int* j, char A[N][N]) {
  while(1) {
    changePos(where, i, j);
    switch(A[*i][*j]){
      case '%':
      case '|': continue;
      case '\\': where ^= 1; continue;
      case '/': where ^= 3; continue;
    }
    break;
  }
}

char solve_r(char A[N][N], int i, int j, int dir, int lcnt, int ccnt);

inline static char mirror45(char A[N][N], int i, int j, int dir, int lcnt, int ccnt) {
  A[i][j] = '/';
  return solve_r(A, i, j, dir^3, lcnt-1, ccnt);
}

inline static char mirror135(char A[N][N], int i, int j, int dir, int lcnt, int ccnt) {
  A[i][j] = '\\';
  return solve_r(A, i, j, dir^1, lcnt-1, ccnt);
}

inline static char mirror90(char A[N][N], int i, int j, int dir, int lcnt, int ccnt) {
  A[i][j] = '|';
  if(solve_r(A, i, j, dir, lcnt, ccnt)) {
    A[i][j] = ' ';
    return 1;
  } else {
    return 0;
  }
}

char solve_r(char A[N][N], int i, int j, int dir, int lcnt, int ccnt) {
  move(dir, &i, &j, A);
  switch(A[i][j]) {
    case ' ':
      if(
        mirror90(A, i, j, dir, lcnt, ccnt)
        || (lcnt > 0 && (
          rand()%2 ?
            mirror45(A, i, j, dir, lcnt, ccnt) || mirror135(A, i, j, dir, lcnt, ccnt)
          :
            mirror135(A, i, j, dir, lcnt, ccnt) || mirror45(A, i, j, dir, lcnt, ccnt)
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
      char r = solve_r(A, i, j, dir, lcnt, ccnt-1);
      A[i][j] = '*';
      return r;
    default:
      return 0;
  }
}

void solve(char A[N][N], int w, int h, int lcnt) {
  int ccnt = 0;
  for(int i=1; i <= h; i++)
    for(int j=1; j <= w; j++)
      if (A[i][j] == '*') ccnt++;

  for(int i=lcnt; i <= lcnt && !solve_r(A, 2, 0, 0, i, ccnt); i++);


}

int main() {
  char A[N][N];
  memset(A, E, N*N);
  int w, h, lcnt;
  scanf("%d %d\n%d\n", &h, &w, &lcnt);
  for(int i=1; i<=h; i++) {
    for(int j=1; j<=w; j++)
      scanf("%c", &A[i][j]);
    scanf("%*c");
  }

  solve(A, w, h, lcnt);

  printf("%d %d\n%d\n", h, w, lcnt);
  for(int i=1; i<=h; i++) {
    for(int j=1; j<=w; j++)
      printf("%c", A[i][j]);
    printf("\n");
  }

  return 0;
}
