#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <queue>

using namespace std;

#define N 102
#define E 'x'

#define max(x, y) ((x) > (y) ? (x) : (y))
#define min(x, y) ((x) > (y) ? (y) : (x))

typedef struct CameFrom {
  int lcnt;
  char direction;
  int cameFrom;
} CameFrom;

inline static CameFrom mkCameFrom(int lcnt, char direction, int cameFrom) {
  CameFrom cf;
  cf.lcnt = lcnt;
  cf.direction = direction;
  cf.cameFrom = cameFrom;
  return cf;
}

// void printBoard(char A[N][N], int w, int h) {
//   for(int i=1; i<=h; i++) {
//     for(int j=1; j<=w; j++)
//       printf("%c", A[i][j]);
//     printf("\n");
//   }
// }

// static void printCF(CameFrom cf[N][N]) {
//   for(int i=0; i<10; i++) {
//     for(int j=0; j<10; j++) {
//       int x, y;
//       dsr(cf[i][j].cameFrom, &x, &y);
//       switch (cf[i][j].lcnt) {
//         case -1: printf("_______ "); break;
//         default: printf("%d!%d:%d:%d ", cf[i][j].lcnt, cf[i][j].direction, x, y);
//       }
//     }
//     printf("\n");
//   }
// }

int sr(int i, int j) {
  return i*N+j;
}

void dsr(int v, int* i, int* j) {
  *i = v/N;
  *j = v%N;
}

inline static char back_dir(char dir) {
  return (dir+2)%4;
}

inline static void move(int dir, int* i, int* j) {
  switch (dir) {
    case 0: (*j)++; break;
    case 1: (*i)++; break;
    case 2: (*j)--; break;
    case 3: (*i)--; break;
  }
}

inline static char visit_one_side_neighbours(
  char A[N][N], int *ri, int *rj, int dir, CameFrom cf[N][N], queue<int> *q, int lcnt
) {
  int i = *ri, j = *rj;
  int cameFrom = sr(i, j);
  while(1) {
    move(dir, &i, &j);
    switch(A[i][j]) {
      case ' ':
        if(cf[i][j].lcnt == -1) {
          cf[i][j] = mkCameFrom(lcnt, dir, cameFrom);
          q->push(sr(i, j));
        }
        continue;
      case '|': continue;
      case '*':
        A[i][j] = '%';
        cf[i][j] = mkCameFrom(lcnt, dir, cameFrom);
        *ri = i; *rj = j;
        return 1;
      case '\\': dir ^= 1; continue;
      case '/': dir ^= 3; continue;
    }
    break;
  }
  return 0;
}

inline static char visit_neighbours(
  char A[N][N], int *i, int *j, int dir, CameFrom cf[N][N], queue<int> *q, int lcnt
) {
  return visit_one_side_neighbours(A, i, j, dir, cf, q, lcnt)
      || visit_one_side_neighbours(A, i, j, back_dir(dir), cf, q, lcnt);
}

int place_mirrors(char A[N][N], int i, int j, CameFrom cf[N][N]) {
  int lcnt = cf[i][j].lcnt;
  for(int k = 0; k < lcnt; k++) {
    char to_dir = cf[i][j].direction;
    int ti = i, tj = j;
    dsr(cf[i][j].cameFrom, &i, &j);
    for(;ti != i || tj != j; move(back_dir(to_dir), &ti, &tj))
      if(A[ti][tj] == ' ')
        A[ti][tj] = '|';
    switch(to_dir ^ cf[i][j].direction) {
      case 1: A[i][j] = '\\'; break;
      case 3: A[i][j] = '/'; break;
    }
  }
  return lcnt;
}

int find_crystal(char A[N][N], int *ri, int *rj, int *dir, int max_lcnt) {
  int i = *ri, j = *rj;
  queue<int> q;
  CameFrom cf[N][N];
  for(int i=0; i < N; i++)
    for(int j=0; j < N; j++)
      cf[i][j] = mkCameFrom(-1, 0, 0);

  if(!visit_one_side_neighbours(A, &i, &j, *dir, cf, &q, 0))
    while(1) {
      dsr(q.front(), &i, &j);
      q.pop();
      if(cf[i][j].lcnt == max_lcnt)
        return -1;
      if(visit_neighbours(A, &i, &j, (cf[i][j].direction+1)%2, cf, &q, cf[i][j].lcnt + 1))
        break;
      if(q.empty())
        return -1;
    }

  *ri = i; *rj = j;
  *dir = cf[i][j].direction;
  return place_mirrors(A, i, j, cf);
}

char solve(char A[N][N], int w, int h, int lcnt) {
  int ccnt = 0;
  for(int i=1; i <= h; i++)
    for(int j=1; j <= w; j++)
      if (A[i][j] == '*') ccnt++;

  for(int i=2, j=0, dir=0; ccnt > 0; ccnt--) {
    int f = find_crystal(A, &i, &j, &dir, lcnt);
    if(f == -1) {
      // printf("solution not found\n");
      break;
    }
    lcnt -= f;
  }

  for(int i=1; i <= h; i++)
    for(int j=1; j <= w; j++)
      switch(A[i][j]) {
        case '%': A[i][j] = '*'; break;
        case '|': A[i][j] = ' '; break;
      }

  return ccnt == 0;
}


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

  if(!solve(A, w, h, lcnt)) {
    brut_solve(A, w, h, lcnt);
  }

  printf("%d %d\n%d\n", h, w, lcnt);
  for(int i=1; i<=h; i++) {
    for(int j=1; j<=w; j++)
      printf("%c", A[i][j]);
    printf("\n");
  }

  return 0;
}
