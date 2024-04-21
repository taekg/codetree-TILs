#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<vector>
#include<cstring>
#include<algorithm>
#include<queue>
using namespace std;

// 예술성을 평가하는 알고리즘을 만들어냈다
// 각 칸의 색깔를 1이상 10 이차의 숫자로 표현하였다

// 각 색깔마다 그룹을 지정한다
// 예술점수는 모든 그룹 쌍의 조화로움의 합으로 정의된다
// 그룹 a와 그룹 b의 조화로움은 
// (a그룹 칸 수 + b그룹 칸 수) X a숫자값 X b숫자값 X a,b서로 맞닿아 있는 변의수
// 따라서 맞닿아 있는 변의 수가 없는 경우는 0이라서 고려하지 않는다

// 지금 현재 맵의 초기 예술 점수를 구한다
// 그림에 대한 회전을 진행
// 회전은 한 가운데로부터 십자 모양과
// 그외의 부분으로 이루어진다.

// 십자모양
// 통쨰로 반시계 90도 회전
// 나머지 부분은 개별적으로 시계로 90도 회전

// 이 과정을 모두 진행 => 1턴
// 3회전을 하며 예술점수의 합을 구하라

// 어떻게 풀것인가?
// 맵의 점수를 구하는 함수 Score()
// 십자가 회전 함수 CrossTurn()
// 그 외의부분 회전 함수 SquareTurn()

struct Pos
{
	int y;
	int x;
};

struct Color
{
	int y;
	int x;
	int C;
};
// 최대로 나올 수 있는 그룹 개수
// 29 * 29 = 841 
vector<Color>cv[900];

int N;
int MAP[29][29];
int visited[29][29];
int grpMAP[29][29];
int side[850][850];

int gCnt;
int TSize;
int dy[4] = { -1,0,1,0 };
int dx[4] = { 0,1,0,-1 };
int turnScore;
int totalScore;

void input() {
	cin >> N;
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			cin >> MAP[i][j];
		}
	}
}

void makeGroup(int y, int x, int grpIdx) {

	visited[y][x] = 1;
	grpMAP[y][x] = grpIdx;
	queue<Pos>q;
	q.push({ y,x });
	int nowC = MAP[y][x];
	cv[grpIdx].push_back({ y,x,nowC });
	while (!q.empty()) {
		Pos now = q.front();
		q.pop();
		for (int d = 0; d < 4; d++)
		{
			int ny = now.y + dy[d];
			int nx = now.x + dx[d];
			if (ny >= N || nx >= N || ny < 0 || nx < 0)continue;
			if (visited[ny][nx] == 1)continue;
			if (MAP[ny][nx] != nowC)continue;
			visited[ny][nx] = 1;
			grpMAP[ny][nx] = grpIdx;
			cv[grpIdx].push_back({ ny,nx,nowC });
			q.push({ ny,nx });
		}
	}
}

void sideCnt() {
	// 각 그룹대로 순서대로 맞닿은 변 탐색
	// grpMAP은 몇번째 그룹이 어디에 채워져 있는지 표시해논 상태이다
	// grpMAP을 통해 각 그룹끼리 맞닿은 변의 개수를 배열로 저장하자

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			int nowG = grpMAP[i][j];
			for (int d = 0; d < 4; d++)
			{
				int ny = i + dy[d];
				int nx = j + dx[d];
				if (ny >= N || nx >= N || ny < 0 || nx < 0)continue;
				if (grpMAP[ny][nx] == nowG)continue;
				side[nowG][grpMAP[ny][nx]]++;
			}
		}
	}
	memset(grpMAP, 0, sizeof(grpMAP));
}

void devScore() {
	// 점수를 구하자
	// 그룹 순서대로 진행한다
	// 1번 그룹에서 맞닿은 그룹이 몇개인지 구한다
	// 그 맞닿은 그룹을 다 했다면
	// 1번 그룹을 계산처리
	// 그럼 2번그룹은 1번과 계산을 추가로 할 필요가 없음
	int complete[850] = { 0, };

	for (int i = 1; i <= gCnt; i++)
	{
		// 해당 gCnt에 해당하는 그룹이랑 맞닿은 그룹만큼 진행한다
		for (int j = 1; j <= gCnt; j++)
		{
			if (complete[j] == 1)continue;
			// 맞닿아 있는 그룹이 있으면 점수 계산 ㄱㄱ
			if (side[i][j] != 0) {
				// i번째 그룹과 j번째 그룹의 점수
				int ijScore = (cv[i].size() + cv[j].size()) * cv[i][0].C * cv[j][0].C * side[i][j];
				turnScore += ijScore;
			}
		}
		complete[i] = 1;
	}
	totalScore += turnScore;
	//cout << turnScore << endl;
	turnScore = 0;
	memset(side, 0, sizeof(side));
	for (int i = 0; i <= gCnt; i++)
	{
		cv[i].clear();
	}
}

void turnCross() {
	// 맵의 한 가운데 있는 십자가 구역을 구하자
	// 맵의 크기는 무조건 홀 수이기 때문이다
	// 만약 맵의 크기가 7이라면 십자가 모양은 3, 3을 기준으로 만들어진다
	// 따라서 맵의 크기에 따라 
	// 십자가의 크기 변수와 그 외의 사각형 변을 저장하자

	TSize = N / 2;

	queue<int>cq;
	// 위 오른쪽 아래 왼쪽 순서대로 queue에 저장
	for (int d = 0; d < 4; d++)
	{
		// 0 1 2 3
		for (int j = 1; j <= TSize; j++)
		{
			int ny = TSize + j * dy[d];
			int nx = TSize + j * dx[d];
			cq.push(MAP[ny][nx]);
		}
	}
	// 왼쪽 위 오른쪽 아래 순서대로 MAP에 채움
	for (int d = 3; d < 7; d++)
	{
		// 3 0 1 2
		for (int j = 1; j <= TSize; j++)
		{
			int ny = TSize + j * dy[d % 4];
			int nx = TSize + j * dx[d % 4];
			MAP[ny][nx] = cq.front();
			cq.pop();
		}
	}
}

void turnSquare(int stY, int stX) {
	// 왼쪽 위 사각형은 그대로 돌린다
	// 나머지 사각형들은 원점으로 이동해준다

	int copyMAP[14][14] = { 0, };
	for (int i = 0; i < TSize; i++)
	{
		for (int j = 0; j < TSize; j++)
		{
			copyMAP[i][j] = MAP[stY + i][stX + j];
		}
	}
	// 시계방향으로 돌린다
	int turnMAP[14][14] = { 0, };
	for (int i = 0; i < TSize; i++)
	{
		for (int j = 0; j < TSize; j++)
		{
			turnMAP[i][j] = copyMAP[TSize - j - 1][i];
		}
	}
	// 맵에 다시 넣는다
	for (int i = 0; i < TSize; i++)
	{
		for (int j = 0; j < TSize; j++)
		{
			MAP[stY + i][stX + j] = turnMAP[i][j];
		}
	}
}

int main() {

	input();
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			if (visited[i][j] == 1)continue;
			gCnt++;
			makeGroup(i, j, gCnt);
		}
	}
	memset(visited, 0, sizeof(visited));
	sideCnt();
	devScore();
	// 여기까지 초기 점수 구하는 과정
	for (int i = 0; i < 3; i++)
	{
		// 총 3번 돌린다
		// 십자가 돌린다
		turnCross();
		// 사각형은 4개를 돌린다
		turnSquare(0, 0);
		turnSquare(TSize + 1, 0);
		turnSquare(0, TSize + 1);
		turnSquare(TSize + 1, TSize + 1);

		// 맵을 돌렸다면
		// 점수를 구하는 과정 ㄱㄱ
		gCnt = 0;
		memset(visited, 0, sizeof(visited));
		for (int k = 0; k < N; k++)
		{
			for (int j = 0; j < N; j++)
			{
				if (visited[k][j] == 1)continue;
				gCnt++;
				makeGroup(k, j, gCnt);
			}
		}
		sideCnt();
		devScore();
	}
	cout << totalScore;

	return 0;
}