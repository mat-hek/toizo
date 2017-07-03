#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <queue>

using namespace std;

#define W (w+2)
#define H (h+2)
#define E 'x'

static int w;
static int h;

#define max(x, y) ((x) > (y) ? (x) : (y))
#define min(x, y) ((x) > (y) ? (y) : (x))

typedef struct CameFrom {
  int lcnt;
  char direction;
  int idx;
} CameFrom;

inline static CameFrom mkCameFrom(int lcnt, char direction, int idx) {
  CameFrom cf;
  cf.lcnt = lcnt;
  cf.direction = direction;
  cf.idx = idx;
  return cf;
}

int sr(int i, int j) {
  return i*W+j;
}

void dsr(int v, int* i, int* j) {
  *i = v/W;
  *j = v%W;
}

void printIdx(int idx) {
  int x, y;
  dsr(idx, &x, &y);
  printf("%d:%d\n", x, y);
}

void printBoard(char* A) {
  for(int i=1; i<H-1; i++) {
    for(int j=1; j<W-1; j++)
      printf("%c", A[sr(i, j)]);
    printf("\n");
  }
}


void printCF(CameFrom* cf) {
  for(int i=0; i<H; i++) {
    for(int j=0; j<W; j++) {
      CameFrom cameFrom = cf[sr(i, j)];
      int x, y;
      dsr(cameFrom.idx, &x, &y);
      switch (cameFrom.lcnt) {
        case -1: printf("_________ "); break;
        default: printf("%d!%d:%02d:%02d ", cameFrom.lcnt, cameFrom.direction, x, y);
      }
    }
    printf("\n");
  }
}

inline static char back_dir(char dir) {
  return (dir+2)%4;
}

inline static int move(int dir, int i) {
  switch (dir) {
    case 0: return i + 1;
    case 1: return i + W;
    case 2: return i - 1;
    case 3: return i - W;
  }
  // *(int*)0 = 0;
  exit(-1);
}

inline static char visit_one_side_neighbours(
  char* A, int *ri, int dir, CameFrom* cf, queue<int> *q, int lcnt, int max_lcnt
) {
  int i = *ri;
  while(1) {
    i = move(dir, i);
    switch(A[i]) {
      case ' ':
        if(cf[i].lcnt == -1 && lcnt < max_lcnt) {
          cf[i] = mkCameFrom(lcnt, dir, *ri);
          q->push(i);
        }
        continue;
      case '|': continue;
      case '%': continue;
      case '*':
        cf[i] = mkCameFrom(lcnt, dir, *ri);
        *ri = i;
        return 1;
      case '\\': dir ^= 1; continue;
      case '/': dir ^= 3; continue;
    }
    break;
  }
  return 0;
}

inline static char visit_neighbours(
  char* A, int *i, int dir, CameFrom* cf, queue<int> *q, int lcnt, int max_lcnt
) {
  return visit_one_side_neighbours(A, i, dir, cf, q, lcnt, max_lcnt)
      || visit_one_side_neighbours(A, i, back_dir(dir), cf, q, lcnt, max_lcnt);
}

static queue<int> place_mirrors(char* A, int i, CameFrom* cf) {
  CameFrom cameFrom = cf[i];
  queue<int> placed_mirrors;
  A[i] = '%';
  for(int k = 0; cameFrom.idx != -1; cameFrom = cf[i], k++) {
    char dir = back_dir(cameFrom.direction);
    while(i != cameFrom.idx) {
      placed_mirrors.push(i);
      i = move(dir, i);
      switch(A[i]) {
        case ' ': A[i] = '|'; break;
        case '/': dir ^= 3; break;
        case '\\': dir ^= 1; break;
      }
    }

    switch(dir ^ cf[cameFrom.idx].direction) {
      case 1: A[i] = '/'; break;
      case 3: A[i] = '\\'; break;
    }
  }
  return placed_mirrors;
}

static void remove_mirrors(char* A, queue<int> placed_mirrors) {
  A[placed_mirrors.front()] = '*';
  placed_mirrors.pop();
  for(; !placed_mirrors.empty(); placed_mirrors.pop()) {
    A[placed_mirrors.front()] = ' ';
  }
}

static char find_crystal(char* A, int i, int dir, int lcnt, int ccnt);

static char find_next_crystal(char* A, int i, int dir, CameFrom* cf, int lcnt, int ccnt) {
  queue<int> mirrors = place_mirrors(A, i, cf);
  if(ccnt > 0 && !find_crystal(A, i, dir, lcnt, ccnt)) {
    remove_mirrors(A, mirrors);
    return 0;
  } else return 1;
}

static char find_crystal(char* A, int i, int dir, int lcnt, int ccnt) {
  queue<int> q;
  CameFrom* cf = (CameFrom*)malloc(W*H*sizeof(CameFrom));
  for(int i=0; i < W*H; i++)
    cf[i] = mkCameFrom(-1, -1, -1);

  if(visit_one_side_neighbours(A, &i, dir, cf, &q, 0, lcnt)
  && find_next_crystal(A, i, dir, cf, lcnt, ccnt - 1)) {
    free(cf);
    return 1;
  }
  while(!q.empty()) {
    i = q.front();
    q.pop();
    if(visit_neighbours(A, &i, cf[i].direction^1, cf, &q, cf[i].lcnt + 1, lcnt)
    && find_next_crystal(A, i, cf[i].direction, cf, lcnt - cf[i].lcnt, ccnt - 1)) {
      free(cf);
      return 1;
    }
  }
  free(cf);
  return 0;
}

static char solve(char* A, int lcnt) {
  int ccnt = 0;
  for(int i=0; i<W*H; i++)
      if (A[i] == '*') ccnt++;

  char found = find_crystal(A, sr(2, 0), 0, lcnt, ccnt);

  for(int i=0; i<W*H; i++)
    switch(A[i]) {
      case '%': A[i] = '*'; break;
      case '|': A[i] = ' '; break;
      case '/':
      case '\\':
        if(!found) A[i] = ' ';
        break;
    }
  return found;
}


inline static void brut_changePos(int where, int* i, int* j) {
  switch (where) {
    case 0: (*j)++; break;
    case 1: (*i)++; break;
    case 2: (*j)--; break;
    case 3: (*i)--; break;
  }
}

inline static void brut_move(int where, int* i, int* j, char* A) {
  while(1) {
    brut_changePos(where, i, j);
    switch(A[sr(*i,*j)]){
      case '%':
      case '|': continue;
      case '\\': where ^= 1; continue;
      case '/': where ^= 3; continue;
    }
    break;
  }
}

static char brut_solve_r(char* A, int i, int j, int dir, int lcnt, int ccnt);

inline static char brut_mirror45(char* A, int i, int j, int dir, int lcnt, int ccnt) {
  A[sr(i, j)] = '/';
  return brut_solve_r(A, i, j, dir^3, lcnt-1, ccnt);
}

inline static char brut_mirror135(char* A, int i, int j, int dir, int lcnt, int ccnt) {
  A[sr(i, j)] = '\\';
  return brut_solve_r(A, i, j, dir^1, lcnt-1, ccnt);
}

inline static char brut_mirror90(char* A, int i, int j, int dir, int lcnt, int ccnt) {
  A[sr(i, j)] = '|';
  if(brut_solve_r(A, i, j, dir, lcnt, ccnt)) {
    A[sr(i, j)] = ' ';
    return 1;
  } else {
    return 0;
  }
}

static char brut_solve_r(char* A, int i, int j, int dir, int lcnt, int ccnt) {
  brut_move(dir, &i, &j, A);
  char r;
  switch(A[sr(i, j)]) {
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
        A[sr(i, j)] = ' ';
        return 0;
      }
    case '*':
      if(ccnt == 1) return 1;
      A[sr(i, j)] = '%';
      r = brut_solve_r(A, i, j, dir, lcnt, ccnt-1);
      A[sr(i, j)] = '*';
      return r;
    default:
      return 0;
  }
}

void brut_solve(char* A, int lcnt) {
  int ccnt = 0;
  for(int i=0; i < W*H; i++)
    if (A[i] == '*') ccnt++;

  for(int i=lcnt-2; i <= lcnt && !brut_solve_r(A, 2, 0, 0, i, ccnt); i++);

}



int main() {
  int lcnt;
  scanf("%d %d\n%d\n", &h, &w, &lcnt);
  char* A = (char*)malloc(W*H);
  memset(A, E, W*H);
  for(int i=1; i<=h; i++) {
    for(int j=1; j<=w; j++)
      scanf("%c", &A[sr(i, j)]);
    scanf("%*c");
  }

  solve(A, lcnt);
  // if(!solve(A, lcnt))
    // brut_solve(A, lcnt);

  printf("%d %d\n%d\n", h, w, lcnt);
  for(int i=1; i<=h; i++) {
    for(int j=1; j<=w; j++)
      printf("%c", A[sr(i, j)]);
    printf("\n");
  }

  return 0;
}
