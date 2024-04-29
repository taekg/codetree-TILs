#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<cstring>
#include<algorithm>
#include<vector>

using namespace std;

// N * N 미로가 존재
// 높이 r
// 오른쪽 c증가
// -1로 입력받기

// 미로는 3가지의 상태로 존재한다
// 빈칸 벽 출구
// 벽은 내구도가 존재하며
// 회전하면 내구도가 -1씩 깍인다.
// 내구도가 0이 되면 빈칸으로 변경된다.
// 참가자는 1초마다 한칸씩 움직인다.
// 움직이는 조건은 다음과 같다

// 최단거리 |x1-x2| + |y1-y2|
// 모든 참가자는 동시에 움직인다.
// 상하좌우로 움직일 수 있다.
// 벽이면 못감
// 움직인 칸은 현재 머물러 있던 칸보다 최단거리가 가까워야한다.
// 움직일 수 있는 칸이 두개이상이라면 무조건 상, 하로 움직이는 것이 우선
// 참가자가 움직일 수 없다면 움직이지 않는다
// 한칸에 2명이상의 참가자가 있을 수 있다.
// 요약하면 산타의 움직임과 똑같이 움직이면 된다.

// 참가자가 이동을 끝낸 후
// 미로가 회전한다.
// 출구와 가장 가까운 참가자를 포함하여 정사각형을 잡는다.
// 조건에 맞는 정사각형이 많으면 왼쪽 위에 있는 정사각형이 우선
// 시계방향으로 90도 회전하며, 회전된 벽은 내구도가 1씩 깎입니다.

// K초동안 위의 과정을 계속 반복한다.
// K초 전에 모든 참가자가 탈출에 성공하면 게임 끝
// 게임이 끝났을 때, 모든 참가자들의 이동거리 합과 출구 좌표를 출력해라
struct person
{
	int y;
	int x;
	int dis;
	bool exit;
};

struct Square {
	int y;
	int x;
	int size;

	bool operator<(Square Right)const {
		if (size < Right.size)return true;
		if (size > Right.size)return false;

		if (y < Right.y)return true;
		if (y > Right.y)return false;

		if (x < Right.x)return true;
		if (x > Right.x)return false;

		return false;
	}
};

// 참가자 위치 벡터
vector<person>pv;
vector<Square>sv;
int N;
int M;
int K;
int MAP[12][12];
int cpMAP[12][12];
int turnMAP[12][12];
int dy[4] = { -1,1,0,0 };
int dx[4] = { 0,0,1,-1 };
int exitCnt;
int totalMove;
int exY;
int exX;

void input() {
	cin >> N >> M >> K;
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			cin >> MAP[i][j];
		}
	}
	for (int i = 0; i < M; i++)
	{
		int y, x = 0;
		cin >> y >> x;
		pv.push_back({ y - 1,x - 1,0,false });
	}
	cin >> exY >> exX;
	exY--;
	exX--;
}

void turnSquare(int stY, int stX, int size) {
	// 정사각형을 만드는 시작점과 사이즈를 이용해서 맵을 돌려서 갱신

	// 해당정사각형 안에 있는 참가자 파악
	for (int i = 0; i < M; i++)
	{
		if (pv[i].exit)continue;
		// 회전하는 정사각형 안에 있다면 회전 시켜서 저장
		if (pv[i].y >= stY && pv[i].y <= stY + size && pv[i].x >= stX && pv[i].x <= stX + size) {
			int my = pv[i].y - stY;
			int mx = pv[i].x - stX;
			int ny = mx;
			int nx = size - my;
			pv[i].y = ny + stY;
			pv[i].x = nx + stX;
		}
	}

	memset(cpMAP, 0, sizeof(cpMAP));
	// 맵의 특정 구역을 0,0으로 시작하는 cpMAP에 저장한다.

	for (int i = 0; i <= size; i++)
	{
		for (int j = 0; j <= size; j++)
		{
			if (MAP[stY + i][stX + j] >= 1 && MAP[stY + i][stX + j] <= 9) {
				cpMAP[i][j] = MAP[stY + i][stX + j] - 1;
			}
			else {
				cpMAP[i][j] = MAP[stY + i][stX + j];
			}
		}
	}

	memset(turnMAP, 0, sizeof(turnMAP));
	// cpMAP을 돌린다.
	for (int i = 0; i <= size; i++)
	{
		for (int j = 0; j <= size; j++)
		{
			turnMAP[i][j] = cpMAP[size - j][i];
		}
	}

	for (int i = 0; i <= size; i++)
	{
		for (int j = 0; j <= size; j++)
		{
			MAP[i + stY][j + stX] = turnMAP[i][j];
		}
	}

	// 탈출구 갱신
	int ey = exY - stY;
	int ex = exX - stX;

	int eY = ex;
	int eX = size - ey;

	exY = eY + stY;
	exX = eX + stX;

}

void move() {

	// 출구까지의 거리를 갱신한다.
	for (int i = 0; i < M; i++)
	{
		// 이미 탈출했다면 컨티뉴
		if (pv[i].exit)continue;
		int Dis = abs(pv[i].y - exY) + abs(pv[i].x - exX);
		pv[i].dis = Dis;
	}

	// 지금 현재 위치와 상하좌우 4방향의 최소 거리를 계산한다.
	for (int i = 0; i < M; i++)
	{
		if (pv[i].exit)continue;
		// 현재 위치에서 출구까지의 거리를 저장

		int MinDis = pv[i].dis;
		for (int d = 0; d < 4; d++)
		{
			int ny = pv[i].y + dy[d];
			int nx = pv[i].x + dx[d];
			if (ny >= N || nx >= N || ny < 0 || nx < 0)continue;
			// 벽이면 가지마라
			if (MAP[ny][nx] >= 1 && MAP[ny][nx] <= 9)continue;
			int findDis = abs(ny - exY) + abs(nx - exX);
			if (findDis < MinDis) {
				MinDis = findDis;
				// 움직여라
				totalMove++;
				pv[i].y = ny;
				pv[i].x = nx;
				// 움직인곳이 탈출구라면
				if (ny == exY && nx == exX) {
					pv[i].exit = true;
					exitCnt++;
				}
				break;
			}
		}
	}

}

void makeSquare() {
	
	int targetY = 0;
	int targetX = 0;

	// 타겟설정했으니
	// 정사각형을 만들고 회전하자
	int stX = 21e8;
	int stY = 21e8;
	int Size = 1;

	// 모든 참가자 벡터를 다 돌면서
	// 정사각형을 만든다.
	// 그 중에 가장 작으며 위쪽에 있고
	// 왼쪽에 있는 정사각형의 시작점과 사이즈를 뽑는다.

	sv.clear();

	for (int i = 0; i < M; i++)
	{
		if (pv[i].exit)continue;
		
		targetY = pv[i].y;
		targetX = pv[i].x;

		if (targetX == exX || targetY == exY) {

			// y값이 같다면
			// 시작점의 x값을 설정한다.
			if (targetY == exY) {
				Size = abs(targetX - exX);

				// 맨 위로 올렸을때 시작점이 맵밖이다
				if (targetY - Size <= 0) {
					// target이 왼쪽에있는경우
					if (targetX < exX) {
						stY = 0;
						stX = targetX;
					}
					else {
						stY = 0;
						stX = exX;
					}
				}

				else {
					// target이 왼쪽에있는경우
					if (targetX < exX) {
						stY = targetY - Size;
						stX = targetX;
					}
					else {
						stY = targetY - Size;
						stX = exX;
					}
				}
			}
			// x값이 같다면
			// 시작점의 y값을 설정한다.
			else {
				Size = abs(targetY - exY);

				// 맨 왼쪽으로 올렸을때 시작점이 맵밖이다
				if (targetX - Size <= 0) {
					// target이 위에있는경우
					if (targetY < exY) {
						stX = 0;
						stY = targetY;
					}
					else {
						stX = 0;
						stY = exY;
					}
				}

				else {
					// target이 위에있는경우
					if (targetY < exY) {
						stX = targetX - Size;
						stY = targetY;
					}
					else {
						stX = targetX - Size;
						stY = exY;
					}
				}
			}

		}
		else {
			// x와 y값이 두개 다 다르다면~
			// 끝점 구하기
			// target과 탈출구가 있을때
			// x y값을 비교해서 무조건 큰 값이 엔드점이 된다.
			// 그리고 정사각형의 크기를 두 점으로 구할 수 있다.

			int endY = 0;
			int endX = 0;
			// targetX와 exX 비교
			if (targetX >= exX) {
				endX = targetX;
			}
			else {
				endX = exX;
			}
			// targetY와 exY비교
			if (targetY >= exY) {
				endY = targetY;
			}
			else {
				endY = exY;
			}

			// end점 ( 정사각형의 맨 오른쪽 아래 )
			// 을 구했으면 정사각형 크기를 구하자
			int difX = abs(targetX - exX);
			int difY = abs(targetY - exY);

			if (difX >= difY) {
				Size = difX;
			}
			else {
				Size = difY;
			}

			// 그럼 시작점을 구할 수 있다.
			// 시작점은 엔드점 - 사이즈
			stY = endY - Size;
			stX = endX - Size;

			if (stY < 0) {
				stY = 0;
			}
			if (stX < 0) {
				stX = 0;
			}
		}

		// 벡터에추가하고 소트 ㄱㄱ
		sv.push_back({ stY, stX, Size });
	}

	sort(sv.begin(), sv.end());

	turnSquare(sv[0].y, sv[0].x, sv[0].size);
}


int main() { 

	input();

	for (int i = 0; i < K; i++)
	{
		// 모든 참가자가 탈출했다면 break;

		move();
		if (exitCnt == M) {
			break;
		}
		makeSquare();
	}

	cout << totalMove << endl << exY + 1 << " " << exX + 1;

	return 0;
}