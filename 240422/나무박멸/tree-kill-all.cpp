#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<vector>
#include<algorithm>
#include<cstring>
using namespace std;

// 전체 맵 n * n
// 제초제는 k 범위만큼 대각선으로 퍼진다
// 벽이 있는 경우 가로막혀서 전파 X

// 인접한 네개의 칸 중 나무가 있는 칸의 수만큼 나무가 성장
// 상하 좌우로 4개의 나무가 존재하면 +4

// 기존의 나무들은 인접한 4개의 칸 중 
// 벽, 다른 나무, 제초제 모두 없는 칸에 번식을 진행한다
// 해당 칸의 나무수가 31이다
// 번식 가능한 칸이 3칸이다
// 그럼 그 세칸으로 10씩 번식하고 나머지는 버린다
// 모든 나무에서 동시에 일어나기 때문에 중복이 가능하다

// 제초제
// 나무가 가장 많이 박멸되는 칸에 제초제를 투여
// 나무가 있는 칸에 제조체를 뿌르면 4개의 대각선 방향으로 k칸만큼 전파
// 전파도중 벽이거나 나무가 없다면 전파 중지
// 제초제를 한번 뿌린 칸에는 c년만큼 제초제가 남아있다
// c+1년에 제초제가 사라진다
// 새로 뿌르면 다시 c년동안 유지
// 죽는 나무의 수가 같은 칸이 있다면 행이 작고 열이 작은 칸에 투여
struct kill
{
	int y;
	int x;
	int killCnt;
	bool operator<(kill right) const {
		if (killCnt > right.killCnt)return true;
		if (killCnt < right.killCnt)return false;

		if (y < right.y)return true;
		if (y > right.y)return false;

		if (x < right.x)return true;
		if (x > right.x)return false;
		return false;
	}
};

struct restKiller
{
	int y;
	int x;
	int rest;
};
vector<kill>kv;
vector<restKiller>rv;

int n;
int m;
int k;
int c;
int MAP[20][20];
int deadBase[20][20] = { 0, };

int dy[4] = { -1,0,1,0 };
int dx[4] = { 0,1,0,-1 };

int diay[4] = { -1,-1,1,1 };
int diax[4] = { -1,1,1,-1 };
int nowYear;
int totalKill;

void input() {

	cin >> n >> m >> k >> c;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			cin >> MAP[i][j];
		}
	}
}

void grow() {
	// 인접한 칸에 해당하는 나무 개수만큼 성장한다
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (MAP[i][j] == 0 || MAP[i][j] == -1)continue;
			int Cnt = 0;
			for (int d = 0; d < 4; d++)
			{
				int ny = i + dy[d];
				int nx = j + dx[d];
				if (deadBase[ny][nx] == 1)continue;
				if (MAP[ny][nx] != 0 && MAP[ny][nx] != -1) {
					Cnt++;
				}
			}
			MAP[i][j] += Cnt;
		}
	}

	// 번식한다
	int breedMAP[20][20] = { 0, };
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{

			if (MAP[i][j] == 0 || MAP[i][j] == -1)continue;
			int Cnt = 0;
			for (int d = 0; d < 4; d++)
			{
				int ny = i + dy[d];
				int nx = j + dx[d];
				if (deadBase[ny][nx] == 1)continue;
				if (ny >= n || nx >= n || ny < 0 || nx < 0)continue;
				if (MAP[ny][nx] != 0) continue;
				Cnt++;
			}
			// 번식가능한 칸의 개수를 구하면
			// 번식할 수 있는 양을 구한다
			int breed = 0;
			if (Cnt >= 1) {
				breed = MAP[i][j] / Cnt;
				for (int d = 0; d < 4; d++)
				{
					int ny = i + dy[d];
					int nx = j + dx[d];
					if (deadBase[ny][nx] == 1)continue;
					if (ny >= n || nx >= n || ny < 0 || nx < 0)continue;
					if (MAP[ny][nx] != 0) continue;
					breedMAP[ny][nx] += breed;
				}
			}
		}
	}
	// 원래 맵에 번식맵을 더한다
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (MAP[i][j] == 0 && breedMAP[i][j] != 0) {
				MAP[i][j] = breedMAP[i][j];
			}
		}
	}
}

void plantKiller() {

	// 어디에 제초제를 뿌릴 것인지 정하자
	// vector로 해당 위치와 죽는 나무의 양을 저장
	int dieTree = 0;
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (MAP[i][j] == 0 || MAP[i][j] == -1)continue;
			dieTree = MAP[i][j];
			for (int d = 0; d < 4; d++)
			{
				int nowy = i;
				int nowx = j;
				int kCnt = 0;
				while (kCnt < k) {
					int ny = nowy + diay[d];
					int nx = nowx + diax[d];
					if (ny >= n || nx >= n || ny < 0 || nx < 0)break;
					if (MAP[ny][nx] == 0 || MAP[ny][nx] == -1)break;
					dieTree += MAP[ny][nx];
					nowy = ny;
					nowx = nx;
					kCnt++;
				}
			}
			kv.push_back({ i,j,dieTree });
		}
	}
	sort(kv.begin(), kv.end());

	// kv의 첫번째 요소의 위치로 제초제를 투여한다
	// 제초제의 영향이 있는 위치를 MAP에서 0으로 만들고
	// 해당 경로를 벡터에 저장하자
	// bool 배열을 하나 선언해서 해당 부분을 true처리
	if (kv.size() >= 1) {
		int stY = kv[0].y;
		int stX = kv[0].x;
		totalKill += kv[0].killCnt;
		kv.clear();
		MAP[stY][stX] = 0;
		rv.push_back({ stY,stX,nowYear + c });
		for (int d = 0; d < 4; d++)
		{
			int nowy = stY;
			int nowx = stX;
			int kCnt = 0;
			while (kCnt < k) {
				int ny = nowy + diay[d];
				int nx = nowx + diax[d];
				if (ny >= n || nx >= n || ny < 0 || nx < 0)break;
				if (MAP[ny][nx] == -1)break;
				MAP[ny][nx] = 0;
				rv.push_back({ ny,nx,nowYear + c });
				nowy = ny;
				nowx = nx;
			}
		}
		int a = 1;
		// 제초제를 뿌리기 전에 년에 따라 제초제 잔여 갱신
		memset(deadBase, 0, sizeof(deadBase));
		for (int i = 0; i < rv.size(); i++)
		{
			if (rv[i].rest <= nowYear)continue;
			deadBase[rv[i].y][rv[i].x] = 1;
		}
	}

}

int main() {

	input();
	for (int i = 1; i <= m; i++)
	{
		nowYear = i;
		// 나무의 성장과 번식
		grow();
		// 제초제
		plantKiller();
	}
	cout << totalKill;
	return 0;
}