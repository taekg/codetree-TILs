#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<cstring>
#include<algorithm>
#include<vector>
using namespace std;

struct Knight
{
    int y;
    int x;
};

// 기사벡터 ( 기사의 영역만 저장 )
vector<Knight>kv[30];
// 밀리지 않는 오더일시 전 상태 저장용 cv
vector<Knight>cv[30];
// 체력 갱신 저장
vector<int>life[30];
// 초기 체력 저장
vector<int>total_life[30];


int L;
int N;
int Q;
int MAP[40][40];

// 재귀를 돌았던 기사 파악
int DAT[30];
// 죽었는지 체크
int die[30];
// 밀렸는지 체크
int isPush[30];

int dy[4] = { -1,0,1,0 };
int dx[4] = { 0,1,0,-1 };

// 오더를 수행할 수 있는지 없는지?
int noWay = 0;

// 재귀로 밀자
void Push(int push, int dir) {

    for (int i = 0; i < kv[push].size(); i++)
    {
        // 방향대로 한칸 밀어보자
        kv[push][i].y += dy[dir];
        kv[push][i].x += dx[dir];
        // 벽이라면 못간다
        if (kv[push][i].y >= L || kv[push][i].x >= L || kv[push][i].y < 0 || kv[push][i].x < 0) {
            noWay = 1;
            break;
        }
        // 장애물이면 못간다
        if (MAP[kv[push][i].y][kv[push][i].x] == 2) {
            noWay = 1;
            break;
        }
    }

    // 종료조건
    if (noWay == 1) {
        return;
    }

    // 밀어보자~~!!!
    for (int k = 0; k < kv[push].size(); k++)
    {
        for (int i = 0; i < N; i++)
        {
            // 오더를 받은 기사는 생각하지 않는다.
            if (push == i)continue;
            // 죽은기사라면 생각하지 않는다.
            if (die[i] == 1)continue;
            // 이미 움직였던 기사는 생각하지 않는다.
            if (DAT[i] == 1)continue;
            for (int j = 0; j < kv[i].size(); j++)
            {
                // 밀린기사가 있다면
                if (kv[push][k].y == kv[i][j].y && kv[push][k].x == kv[i][j].x) {
                    DAT[i] = 1;
                    isPush[i] = 1;
                    // 밀린기사로 또 밀자~
                    Push(i, dir);
                    DAT[i] = 0;
                }
            }
        }
    }
}

void input() {

    cin >> L >> N >> Q;
    // 맵 입력
    for (int i = 0; i < L; i++)
    {
        for (int j = 0; j < L; j++)
        {
            cin >> MAP[i][j];
        }
    }

    // 각 기사의 영역을 받고 체력을 받는다
    for (int i = 0; i < N; i++)
    {
        int r, c, h, w, k = 0;
        cin >> r >> c >> h >> w >> k;
        for (int u = 0; u < h; u++)
        {
            // 각 기사의 영역 저장
            for (int j = 0; j < w; j++)
            {
                kv[i].push_back({ r + u - 1,c + j - 1 });
            }
        }
        life[i].push_back(k);
        total_life[i].push_back(k);
    }

    // 명령만큼 돌아라
    for (int i = 0; i < Q; i++)
    {
        int toOrd, order = 0;
        cin >> toOrd >> order;
        // 재귀함수 들어가기 전에
        // 현재상태 저장

        // 오더를 받는 기사가 이미 죽었다면 무시
        if (die[toOrd - 1] == 1)continue;

        // 재귀 들어가기 전 현재 기사들 위치 저장
        for (int c = 0; c < N; c++)
        {
            cv[c].clear();
        }
        for (int k = 0; k < N; k++)
        {
            for (int j = 0; j < kv[k].size(); j++)
            {
                cv[k].push_back({ kv[k][j].y, kv[k][j].x });
            }
        }
        noWay = 0;
        Push(toOrd - 1, order);
        memset(DAT, 0, sizeof(DAT));
        // 결국 밀리지 않으면 원상복구
        if (noWay == 1) {

            memset(isPush, 0, sizeof(isPush));
            for (int c = 0; c < N; c++)
            {
                kv[c].clear();
            }

            for (int k = 0; k < N; k++)
            {
                for (int j = 0; j < cv[k].size(); j++)
                {
                    kv[k].push_back({ cv[k][j].y, cv[k][j].x });
                }
            }
        }
        // 밀렸다면 데미지 주고 원상복구 X
        else {

            for (int k = 0; k < N; k++)
            {
                // 안밀린 기사라면 생각 X
                if (isPush[k] != 1)continue;
                // 죽은 기사면 생각하지 X
                if (die[k] == 1)continue;
                int dangerCnt = 0;
                for (int j = 0; j < kv[k].size(); j++)
                {
                    if (MAP[kv[k][j].y][kv[k][j].x] == 1) {
                        dangerCnt++;
                    }
                }
                life[k][0] -= dangerCnt;
                if (life[k][0] <= 0)die[k] = 1;
            }
            memset(isPush, 0, sizeof(isPush));
        }
    }
}

int main() {

    input();
    int ans = 0;
    //cout << endl;
    for (int i = 0; i < N; i++)
    {
        // 죽으면 데미지 계산할 필요가 없음
        if (die[i] == 1)continue;
        //cout <<i<<" " << total_life[i][0] - life[i][0] << endl;
        ans += total_life[i][0] - life[i][0];
    }

    cout << ans;
    return 0;
}