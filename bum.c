#include <stdio.h>
#include <string.h>

#define N 102
#define E 'x'

// void printBoard(char A[N][N], int w, int h) {
//   for(int i=1; i<=h; i++) {
//     for(int j=1; j<=w; j++)
//       printf("%c", A[i][j]);
//     printf("\n");
//   }
// }

void changePos(int where, int* i, int* j) {
  switch (where) {
    case 0: (*j)++; break;
    case 1: (*i)++; break;
    case 2: (*j)--; break;
    case 3: (*i)--; break;
  }
}

void move(int where, int* i, int* j, char A[N][N]) {
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

int reverseDir(int dir) {
  return (dir+2)%4;
}

char mirror(int from, int to) {
  if(from == to) return '|';
  else if((from ^ to) == 1) return '\\';
  else return '/';
}

char solve_r(char A[N][N], int i, int j, int dir, int lcnt, int ccnt) {
  move(dir, &i, &j, A);
  switch(A[i][j]) {
    case ' ':
      if(lcnt == 0) {
        A[i][j] = '|';
        if(solve_r(A, i, j, dir, lcnt, ccnt)) {
          A[i][j] = ' ';
          return 1;
        }
      } else {
        for(int goTo=0; goTo < 4; goTo++)
          if (goTo != reverseDir(dir)) {
            char m = mirror(dir, goTo);
            A[i][j] = m;
            if(m == '|' && solve_r(A, i, j, goTo, lcnt, ccnt)) {
              A[i][j] = ' ';
              return 1;
            } else if(solve_r(A, i, j, goTo, lcnt-1, ccnt)) {
              return 1;
            }
          }
      }
      A[i][j] = ' ';
      return 0;
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
