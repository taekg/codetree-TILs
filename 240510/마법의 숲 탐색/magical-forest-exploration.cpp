#define _CRT_SECURE_NO_WARNINGS
#include<algorithm>
#include<iostream>
#include<cstring>
#include<vector>
#include<queue>
using namespace std;

struct Pos
{
	int y;
	int x;
};

struct Golum
{
	int y;
	int x;
	int dir;
};
vector<Golum>gv;

int R;
int C;
int K;
int MAP[73][70];
int Exit[73][70];
bool reset = false;
int ans;

int dy[4] = {-1,0,1,0};
int dx[4] = {0,1,0,-1};

// 정령은 위에서 아래로 내려온다
// 남쪽 방향에 아무것도 없으면 남쪽 ㄱㄱ
// 남쪽으로 이동 못하면 서쪽 
// 서쪽으로도 이동 못하면 동쪽
// 서쪽과 동쪽으로 이동할때는 회전하기 때문에 골렘의 출구가 변경
// 서쪽, 동쪽, 남쪽 모두 이동 불가능 하면 멈춘다

// 골렘이 못움직이는 상황이 되면
// 정령은 골렘을 타고 최대한 밑으로 이동한다
// 골렘의 출구가 다른 골렘과 인접해있으면 다른 골렘으로 이동가능
// move 함수 제작
// 남쪽으로 이동 아래 3칸 확인
// 서쪽으로 대각선 이동 왼쪽 5칸 확인
// 동쪽으로 대각선 이동 오른쪽 5칸 확인
// 어느쪽도 불가능 하면 이동 멈춤
// 근데 정령이 맵 밖이면 맵 리셋
// 이때 맵의 왼쪽 오른쪽 아래쪽은 예외 처리를 하되
// 위쪽칸들은 예외 처리를 하지 마라

// move 함수가 종료되면 정령이 움직인다
// 위를 제외하고 해당 출구에서 완탐
// 가장 밑으로 갈 수 있는 케이스를 구한다
// 일단 지금 현재 위치에 있는 골렘의 출구로 이동한다
// 다른 골렘으로 이동할 수 있냐? 이동
// 다른 골렘으로 이동 못한다 => 그럼 그냥 그 골렘의 밑으로 이동

// 아니면 조건 걸어서 플필로 해결
// 플필 맵에서 가장 밑에 있는 요소를 구한다

void input() {

	cin >> R >> C >> K;
	gv.push_back({ 0,0,0 });
	for (int i = 0; i < K; i++)
	{
		int col, dir = 0;
		cin >> col >> dir;
		gv.push_back({ 1,col-1,dir });
	}
}


void moveGol(int idx) {

	while (true) {
		// idx 번째 골렘이 탐색한다
	    // 남쪽 고려
		int southdir[3][2] = { {1,-1},{1,1},{2,0} };
		bool isSouth = true;
		for (int i = 0; i < 3; i++)
		{
			int ny = gv[idx].y + southdir[i][0];
			int nx = gv[idx].x + southdir[i][1];
			if (ny >= R+3 || nx >= C || nx < 0) {
				isSouth = false;
				break;
			}
			if (MAP[ny][nx] != 0) {
				isSouth = false;
				break;
			}
		}
		// 남쪽으로 이동가능하면~
		if (isSouth) {
			// 이동해라
			gv[idx].y++;
			continue;
		}

		// 서쪽 대각선 고려
		int westdir[5][2] = { {-1,-1},{0,-2},{1,-2},{1,-1},{2,-1} };
		bool isWest = true;
		for (int i = 0; i < 5; i++)
		{
			int ny = gv[idx].y + westdir[i][0];
			int nx = gv[idx].x + westdir[i][1];
			if (ny >= R+3 || nx >= C || nx < 0) {
				isWest = false;
				break;
			}
			if (MAP[ny][nx] != 0) {
				isWest = false;
				break;
			}
		}
		// 서쪽으로 이동가능하면~
		if (isWest) {
			// 이동해라
			gv[idx].y++;
			gv[idx].x--;
			// 서쪽이면 출구방향이 -1
			if (gv[idx].dir == 0) {
				gv[idx].dir = 3;
			}
			else {
				gv[idx].dir--;
			}
			continue;
		}

		// 동쪽 대각선 고려
		int eastdir[5][2] = { {-1,1},{0,2},{1,2},{1,1},{2,1} };
		bool isEast = true;
		for (int i = 0; i < 5; i++)
		{
			int ny = gv[idx].y + eastdir[i][0];
			int nx = gv[idx].x + eastdir[i][1];
			if (ny >= R+3 || nx >= C || nx < 0) {
				isEast = false;
				break;
			}
			if (MAP[ny][nx] != 0) {
				isEast = false;
				break;
			}
		}
		// 동쪽으로 이동가능하면~
		if (isEast) {
			// 이동해라
			gv[idx].y++;
			gv[idx].x++;
			// 동쪽이면 출구 방향이 +1
			gv[idx].dir = (gv[idx].dir + 1) % 4;
			continue;
		}

		// 남쪽 서쪽 동쪽 어느쪽으로도 이동하지 못한다면
		// 멈춰라
		if (!isSouth && !isWest && !isEast) {
			// 이때 골렘의 몸이 맵 밖에 위치한다면
			// 맵 리셋
			bool isOut = false;

			for (int d = 0; d < 4; d++)
			{
				int oy = gv[idx].y + dy[d];
				int ox = gv[idx].x + dx[d];
				if (oy <= 2 || oy>=R+3 || ox <0 || ox >=C) {
					isOut = true;
					break;
				}
			}

			// 이때 정령의 위치가 맵 위라면
			// 맵 리셋
			if (isOut) {
				memset(MAP, 0, sizeof(MAP));
				memset(Exit, 0, sizeof(Exit));
				reset = true;
				break;
			}
			// 아니라면 이동 멈추고
			// 맵에 골렘 순서에 맞게 표시
			else {
				MAP[gv[idx].y][gv[idx].x] = idx;
				for (int d = 0; d < 4; d++)
				{
					int ny = gv[idx].y + dy[d];
					int nx = gv[idx].x + dx[d];
					MAP[ny][nx] = idx;

					if (gv[idx].dir == d) {
						Exit[ny][nx] = 1;
					}
				}
				break;
			}
		}
	}
}

void moveSoul(int idx) {

	// 골룸이 이동을 마쳤다면
	// 해당 정령이 이동할 수 있는 곳을 파악한다
	int fillMAP[73][70] = { 0, };
	int MaxY = 0;
	queue<Pos>q;
	q.push({ gv[idx].y, gv[idx].x });
	fillMAP[gv[idx].y][gv[idx].x] = 1;
	while (!q.empty()) { 

		Pos now = q.front();
		MaxY = max(MaxY, now.y);
		q.pop();
		for (int d = 0; d < 4; d++)
		{
			int ny = now.y + dy[d];
			int nx = now.x + dx[d];
			if (ny >= R + 3 || nx >= C || ny < 0 || nx < 0)continue;

			// 빈칸으로는 이동할 수 없다
			if (MAP[ny][nx] == 0)continue;
			// 이미 갔던 곳이면 가지마라
			if (fillMAP[ny][nx] == 1)continue;

			// 지금 있는 위치가 문이면 다른 골렘으로 갈 수 있다
			if (Exit[now.y][now.x] == 1) {
				fillMAP[ny][nx] = 1;
				//MaxY = max(MaxY, now.y);
				q.push({ ny,nx });
			}
			// 문이 아니라면 다른 골렘으로 이동 불가능
			else {
				if (MAP[ny][nx] != MAP[now.y][now.x])continue;
				fillMAP[ny][nx] = 1;
				//MaxY = max(MaxY, now.y);
				q.push({ ny,nx });
			}
		}
	}
	ans += (MaxY-2);
	//cout << idx << "번째 : " << MaxY - 2 << endl;
}

int main() {

	input();
	for (int i = 1; i <= K; i++)
	{
		reset = false;
		if (i == 825) {
			int a = 1;
		}
		moveGol(i);
		if (!reset) {
			moveSoul(i);
		}
	}
	cout << ans;

	return 0;
}