---
# 📅 2024.08.01
📋 진행 사항
  * 실제 통로의 너비와 agent의 크기 차이가 과도하게 큼에도 그냥 통과해버릴 수 있던 문제를 수정 중입니다.
  * 아직 구현 및 적용이 완료되지 않아 결과 자료는 없지만 통로의 너비를 계산할 수 있는 방법을 찾던 중 [Efficient Triangulation-Based Pathfindin. (Douglas Jon Demyen)](https://skatgame.net/mburo/ps/thesis_demyen_2006.pdf#page=43)의 4.1절에 제안된 알고리즘을 알게 돼, 적용 중입니다.
    * 통과하고자 하는 두 edge가 이루는 각이 직,둔각 이상인 경우, 나머지 한 edge가 막혀있는 경우, 그렇지 않은 경우 등 여러 경우에 통로의 너비를 측정할 수 있는 알고리즘을 제안합니다.
    * 아래는 현재 구현 및 적용 중인 의사코드입니다.
      
      ![image](https://github.com/user-attachments/assets/c30d01ab-1e0f-4295-8192-7081b72b5302)
      ![image](https://github.com/user-attachments/assets/7f73f4ac-d5e1-4735-8795-b42aeae4822e)
      ![image](https://github.com/user-attachments/assets/8d1cd9db-53e5-4755-bff2-e29c459cfc6e)

    * 통로의 너비가 잘못 측정되는 경우가 있어 파악 및 해결중입니다.

⚽ 이후 계획
  * 오늘은 위 사항과 함께 obstacle과의 충분한 거리를 유지할 수 있도록 경로 선택 전반의 과정을 개선할 계획입니다.

---
* ~~240801 생각 메모...~~
  * ~~IsOut에서 radius를 고려해 충돌 여부 확인하는 건 무한루프 발생... neighbor가 nullptr일 때만 적용하면 얇은 neighbor를 보유한 cell에서 충돌 결과가 명확하지 않음...~~
    * ~~경로로 선택된 cell내에서만 외곽충돌을 검출하도록 하면 충분히 가능할 수도??->안됨.~~
    * ~~해답은 논문 4.1과 4.2절에 있는것 같다. 고려해보자.~~
      
---
# 📅 2024.07.31
📋 진행 사항
  * agent가 obstacle의 경계영역과 충돌하는 경우, 진행이 막히는 현상을 수정했습니다.
    * 수정 전 문제 현상은 아래와 같습니다. 경계선을 넘어 해당 방향으로 진행할 수 없도록 제한했습니다.
    
      ![FPS_61-DEBUG2024-07-3114-17-35-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/b5649b06-4661-446d-9751-0b31e628eb5f)

    * 아래는 수정 이후의 모습입니다. 탈출 방향으로 slide vector를 계산해 현재 위치와 cell을 다시 지정했습니다.
   
      ![FPS_61-DEBUG2024-07-3114-15-54-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/5e74c9d1-7ff5-4881-ad4c-f12d40d33061)

    * 간략하게 다음과 같이 표현할 수 있습니다. 이외에도 다시 셀 검색을 수행하는 등 몇 가지 작업을 추가해 현재 위치한 셀을 추적하는데 오류가 발생하지 않도록 구현했습니다.
 	```
  	...
  
 	// 이전 위치 + 이동 방향 - (move.Dot(탈출edge.normal) * (탈출edge.normal)
	vPosition = vPrePos + vDir - (EPSILON + vDir.Dot(vPassedLine)) * vPassedLine;
	m_pCurrentCell = FindCellByPosition(vPosition);
  
  	...
 	```  
    * 추가로, 관련된 코드 일부를 정리했습니다.

  * 좀 더 복잡한 환경에서의 path finding을 테스트하기 위해 맵을 더 복잡하게 구성할 필요가 있었습니다. 적합한 3D mesh 오픈리소스를 구하는 것이 쉽지 않은데다가, 필요에 따라 맵을 어느 정도 커스텀 가능해야 이후에도 좀 더 유연하게 작업할 수 있을 것이라 생각했습니다.
    * 따라서 아래와 같은 이미지를 제작해 navigation mesh 생성에 사용해 보기로 했습니다.

      ![testmaze](https://github.com/user-attachments/assets/25e38fb0-00f6-49b0-bfdd-3c400b71d8f6)

    * 이미지를 height map으로 활용해 terrain을 생성했습니다.

      ![image](https://github.com/user-attachments/assets/cd5b65c9-f7a6-4033-a924-daebd25923ad)

    * 이전에 제작했던 기능을 개선해 이번엔 모든 언덕 지형에 대해 한 번에 outline들을 생성, 확장 및 단순화 등의 작업을 수행한 뒤, triangulation을 적용해 최종 navigation mesh를 제작했습니다.

      ![image](https://github.com/user-attachments/assets/11c7e984-ed3c-4721-be33-f8018ee1bb07)

    * path finding은 아래와 같이 수행됩니다.

      ![FPS_61-DEBUG2024-08-0110-34-37-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/4fa597d2-74e9-4d0c-8b93-dc04fd4f1f5f)

⚠️ 발견된 문제
  * 해당 지형을 통해 제작된 네비게이션 메쉬를 관찰해보니 4주차 회의에서 피드백으로 말씀해 주셨던 문제점들을 더 명확하게 이해할 수 있었습니다.
    * 아래와 같이 좁고 긴 통로가 형성되는 지역에서는 agent의 크기가 실제 통로의 너비보다 크더라도 길게 형성되는 portal의 길이 때문에 경로로 선택이 될 수 있습니다.
      
    ![image](https://github.com/user-attachments/assets/aed8e6ed-6eff-410b-b13b-26db11abe5bf)

⚽ 이후 계획
  * 오늘은 위 문제를 해결해 agent가 실제로 이동이 가능한 지역만을 경로로 선택할 수 있도록 개선할 계획입니다.
  
---
# 📅 2024.07.30
📋 진행 사항
  * 아래와 같이 경로가 심각하게 우회되는 현상을 수정했습니다.

    ![FPS_61-DEBUG2024-07-3017-58-21-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/36be4005-1744-4934-b63d-5274f0decc6e)

    * 원인은 어느정도 미리 파악을 했으나 재귀적으로 실행되는 구조가 많아 정확한 문제 발생 지점을 찾는데 많은 시간을 소요했습니다.
    * 아래와 같이 g-value계산에 사용된 entry 노드를 잘못 지정함으로써 g-value의 오차가 매우 커지는 문제가 원인이었던 것으로 파악됐습니다.

      ![image](https://github.com/user-attachments/assets/a65fcfd8-bb22-4ee0-9648-c161195655cd)

    * 수정된 결과는 아래와 같습니다. 직선거리에서 급격히 우회하는 문제를 수정하고 어느정도 일관성 높은 경로를 계산할 수 있었습니다.
      
      ![FPS_61-DEBUG2024-07-3018-27-09-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/1e67ae6e-0041-4198-851d-3558532bdb44)

    * 지나갈 수 없는 영역과의 경게 부분에서 원활히 진행할 수 없는 현상이 있어 수정 중입니다.

⚠️ 발견된 문제
  * agent의 size가 작은 경우 obstacle의 경계영역과 충돌하는 경우가 발생하는데, 이 때 진행이 막히거나 통과해버리는 현상이 발생해 수정 중입니다.

⚽ 이후 계획
  * 말씀해 주신 주간 회의 피드백 내용을 토대로 더 다양한 환경에서 경로 탐색을 수행할 수 있도록 테스트 환경을 구성할 계획입니다.

---
# 📅 2024.07.29
  * 29일 진행 사항은 4주차 주간 회의록의 후반 g-value 테스트 부분에 함께 포함돼 있습니다.
    * https://equal-paw-e8f.notion.site/4-025bff362b2649668d2dccf85cfd482b?pvs=4
    
---
# 📅 2024.07.26
📋 진행 사항
  * agent의 크기를 고려해 통과할 수 있는 구역만을 경로로 선택하도록 구현했습니다.
    * 통과하게 될 edge(portal)가 agent의 지름보다 작다면 우선순위 큐에 저장하지 않도록 구현했습니다.

    ![FPS_61-DEBUG2024-07-2910-17-27-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/222e9b23-906b-46fe-8d99-26d06bf38f1a)
    
  * 도착지점이 obstacle 영역 내에 있다면, 근처의 유효한 영역을 대신 목적지로 지정하도록 구현했습니다.
    * 지정한 도착 지점이 obstacle 영역의 내에 있다면 인접한 위치의 cell을 도착 지점으로 변경할 수 있도록 구현했습니다.

    ![FPS_61-DEBUG2024-07-2910-19-29-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/98acc55a-4ce9-430e-bdbd-f2cdfe74ec93)
  
  * 맵의 전체 구역을 grid로 분할 해, grid 내에서 빠르게 cell을 탐색할 수 있도록 구현했습니다.
    * 각각의 cell들은 위치를 key값으로 하는 AABB 영역에 포함되도록 multi-map 구조로 저장했습니다.
    * 영상에서 보이는 격자는 grid의 AABB 크기와 동일합니다.

  * 더 나은 경로를 탐색할 수 있는 방법을 위해 추가 학습중입니다. (Efficient Triangulation-Based Pathfindin. (Douglas Jon Demyen))

⚠️ 발견된 문제
  * 이전에 기록했던 문제들을 아직 해결하지 않은 상태입니다.(도착지점에서의 경로 문제 등)

⚽ 이후 계획
  * 위 논문에서 제안한 방법을 참고해 경로의 g값을 계산하는 방식을 개선해볼 계획입니다.
    * 아래와 같이 여러 방법을 통해 계산한 g-cost를 비교함으로써 경로 개선 여부를 확인해보려합니다.(5.5 Triangulation A* (TA*)의 일부입니다)        
      ![image](https://github.com/user-attachments/assets/b5edb17b-046e-4874-bb9e-3c95b1c8bd70)

---
# 📅 2024.07.25
📋 진행 사항
  * Simple Stupid Funnel 알고리즘을 구현해 경로를 단순화했습니다. (도착지점 부근에서 약간의 문제가 있습니다.)
    * 다음과 같은 과정을 거쳐 경로를 단순화할 수 있습니다.
      * waypoint와 portal(통과할 edge)의 좌우 정점을 연결한 선분을 점차 좁혀나가며 corner를 찾고, 도착지점까지 반복적으로 탐색을 수행하는 알고리즘입니다.
      * 과정을 간략히 묘사한 자료입니다.
        
        ![image](https://github.com/user-attachments/assets/f44801d9-f42e-48ee-9c00-0f423837af28)
        
	```
	void CAgentController::SSF()
	{
		Vec3 vPortalApex = m_pTransform->GetPosition();		// 초기 상태
		Vec3 vPortalLeft = m_pTransform->GetPosition();
		Vec3 vPortalRight = m_pTransform->GetPosition();

		_int iApexIndex = 0;
		_int iLeftIndex = 0;
		_int iRightIndex = 0;

		m_dqWayPoints.push_back(vPortalApex);

		for (_int i = 1; i < m_dqPortals.size(); ++i)
		{
			const Vec3& vLeft = m_dqPortals[i].first;
			const Vec3& vRight = m_dqPortals[i].second;

			if (TriArea2x(vPortalApex, vPortalRight, vRight) <= 0.0f)	// 시작점, 현재 portal의 right, 다음 portal의 right 위치가 구성하는 넓이가 0 이하,
 			{								// 즉 다음 portal의 right를 연결한 선이 외부를 지나지 않는지 확인 -> 그림 E
				if (vPortalApex == vPortalRight || TriArea2x(vPortalApex, vPortalLeft, vRight) > 0.0f)
				{
					vPortalRight = vRight;			// funnel 당기기 -> 그림 B, C, D
					iRightIndex = i;
				}
				else
				{
					m_dqWayPoints.push_back(vPortalLeft);	// Right가 Left를 넘었다면 Left를 waypoint에 추가 -> 그림 F
								
					vPortalApex = vPortalLeft;		// L을 새로운 시작점으로 -> 그림 G
					iApexIndex = iLeftIndex;

					vPortalRight = vPortalApex;		// 해당 지점으로 초기화
					iRightIndex = iApexIndex;

					i = iApexIndex;				// 재시작할 인덱스
					continue;
				}
			}

			if (TriArea2x(vPortalApex, vPortalLeft, vLeft) >= 0.0f)
			{
				if (vPortalApex == vPortalLeft || TriArea2x(vPortalApex, vPortalRight, vLeft) < 0.0f)
				{
					vPortalLeft = vLeft;
					iLeftIndex = i;
				}
				else
				{
					m_dqWayPoints.push_back(vPortalRight);

					vPortalApex = vPortalRight;
					iApexIndex = iRightIndex;

					vPortalLeft = vPortalApex;
					iLeftIndex = iApexIndex;

					i = iApexIndex;
					continue;
				}
			}
		}	

		m_dqWayPoints.push_back(m_vDestPos);
	}
	```
        
      * 구현 결과는 아래와 같습니다.

	![FPS_61-DEBUG2024-07-2610-39-55-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/c32573eb-4923-4c92-a0bb-9563f8aaf942)
        
  * AStar에 불필요한 자료구조가 사용돼 개선중입니다.


⚠️ 발견된 문제
  * 목적지 cell에서 corner를 방문한 후 destination position으로 도착합니다. 전체적인 경로에 큰 문제는 없지만 도착지점 부근에서의 경로는 개선해야할 것 같습니다.
  * 아직 obstacle과 부딪혔을 때는 이동할 수 없습니다. (위의 영상은 임시로 obstacle을 무시하도록 설정해, 일부 통과한 cell의 색이 바뀌지 않습니다.)

⚽ 이후 계획
  * 지금은 agent의 크기를 고려하지 않아 좁은 구간도 자유롭게 통과할 수 있습니다. 이 부분을 반영해 경로를 결정할 수 있도록 구현할 계획입니다.
  
---
# 📅 2024.07.24
📋 진행 사항
  * obstacle 데이터 저장시, 오브젝트 데이터가 저장되지 않아 mesh를 load하지 않던 문제를 해결했습니다.
  * agent가 삼각형 cell 영역을 벗어나지 않고 이동할 수 있도록 구현했습니다.
  * 기본적인 형태의 A* 알고리즘을 구현했습니다.
    * Agent는 경로로 탐색된 삼각형의 무게중심 좌표를 waypoint로 설정해 이동합니다.
    * 경로가 심각한 지그재그 형태로 나타나기 때문에 매끄러운 형태로 다듬기 위해 관련 알고리즘과 자료를 학습중입니다.
    * 현재 상태는 아래와 같습니다. 경로로 탐색된 cell은 하늘색, 통과하게 되는 portal(edge)은 파란색으로 표시하고 있습니다. portal은 이후 경로 개선에 사용하기 위해 함께 저장했습니다.
      * 거리 척도 계산은 우선 유클리드 거리(sqrt(pow(x, 2) + pow(z, 2)))로 계산하고 있습니다. 다른 방법을 함께 적용해볼 계획입니다.
      * obstacle : 150+ / cell : 1500+
        
        ![FPS_61-DEBUG2024-07-2510-23-41-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/851d44b2-4b3d-429a-988c-b7659c21ba9b)

        ```
		_bool CAgentController::AStar()
		{
			priority_queue<PQNode, vector<PQNode>, greater<PQNode>> pqOpen;
			map<CellData*, pair<CellData*, LINES>> mapPath;
			map<CellData*, _float> mapCost;
			set<CellData*> setClosed;
			m_vecPath.clear();

			{	// start node
				Vec3 vStartPos = m_pTransform->GetPosition();

				_float g = 0.0f;
				Vec3 vDistance = m_vDestPos - vStartPos;
				_float h = vDistance.Length();

				pqOpen.push(PQNode{ g + h, g, m_pCurrentCell });
				mapCost[m_pCurrentCell] = 0.0f;
				mapPath[m_pCurrentCell] = pair(nullptr, LINE_END);
			}

			while (false == pqOpen.empty())
			{
				PQNode tNode = pqOpen.top();

				if (tNode.pCell == m_pDestCell)
				{
					pair<CellData*, LINES> pairCell(m_pDestCell, LINE_END);
					while (nullptr != pairCell.first)
					{
						m_vecPath.push_back(pairCell);
						pairCell = mapPath[pairCell.first];
					}

					return true;
				}

				pqOpen.pop();
				setClosed.emplace(tNode.pCell);

				for (uint8 i = LINE_AB; i < LINE_END; ++i)
				{
					CellData* pNeighbor = tNode.pCell->pNeighbors[i];
					if (nullptr != pNeighbor && 0 == setClosed.count(pNeighbor))
					{
						mapCost[pNeighbor] = mapCost[tNode.pCell] + CellData::CostBetween(tNode.pCell, pNeighbor);
						mapPath[pNeighbor] = pair(tNode.pCell, (LINES)i);

						_float g = mapCost[pNeighbor];
						pqOpen.push(PQNode{ g + CellData::HeuristicCost(pNeighbor, m_vDestPos), g, pNeighbor });
					}
				}
			}

			return false;
		}
      	```
  
⚠️ 발견된 문제
  * 당장은 waypoint를 dell의 무게중심으로 지정한 상태이기 때문에 경로가 매우 불안정합니다.
  * 도착 지점이 포함된 cell을 모든 cell에 대한 linear search로 탐색하고 있어 개선이 필요합니다.
  * navmesh에 포함되지 않는 영역으로 이동 명령 시 우선 무시하도록 구현했습니다. 가장 가까운 영역을 탐색하거나, navmesh 외부와의 경계선에서의 슬라이딩과 같은 세부적인 부분은 아직 구현하지 않았습니다.
  
⚽ 이후 계획
  * Simple Stupid Funnel 알고리즘을 통해 이를 개선하고자 합니다.
  * Efficient Triangulation-Based Pathfindin. (Douglas Jon Demyen) 등의 자료를 참고하고 있습니다.
  * 더 좋은 경로를 발견할 수 있는 휴리스틱을 시도해보고자 합니다.
  * 아직은 agent의 크기는 고려하지 않은 채 경로를 탐색하고 있습니다. 경로 단순화 이후에 시도해보려합니다.

---
# 📅 2024.07.23
📋 진행 사항
  * 경로탐색 구현을 위해 agent를 배치하고 navigation mesh 위에서 이동할 수 있도록 구현중입니다.
  * 간단한 sphere형태의 agent를 배치했습니다. agent의 현재 위치부터 마우스로 지정한 도착 지점을 향해 이동하도록 구현할 계획입니다.
  * 삼각형 네비게이션 메쉬에서 경로 탐색 알고리즘을 적용한 예를 다시 찾아보며 학습중입니다.
  * 툴에서 더 이상 사용되지 않는 legacy 코드를 한 차례 더 정리했습니다.
  * 경로 탐색 구현은 다음과 같은 자료를 참고해 접근해 볼 계획입니다.
    * https://jceipek.com/Olin-Coding-Tutorials/pathing.html#funnel-algorithm

⚠️ 발견된 문제
  * ~~셀을 벗어난 경우 정상적으로 현재 셀에 대한 추적이 진행되지 않는 경우가 있어 수정중입니다.~~ -> 초기 배치 시, 셀을 벗어난 곳에 배치한 경우 현재 위치를 파악하지 못해 발생한 문제로 확인됐습니다.

⚽ 이후 계획
  * ~~우선 정상적으로 네비게이션 메쉬 위에서 agent가 움직일 수 있도록 수정할 계획입니다.~~
  * agent와 navigation mesh와의 상호작용이 확인 되면, A* 를 이용해 기본적인 형태의 경로탐색을 구현할 예정입니다.
  * 경로 최적화와 성능 개선은 우선 A* 적용 후에 더 고민해보록 할 계획입니다.

---
https://equal-paw-e8f.notion.site/3-ce15dfe28bc74c5fac609bf0340a0c8f?pvs=4

---
# 📅 2024.07.22
📋 진행 사항
  * obstacle이 될 수 있는 일부 3D mesh의 obstacle data를 xml로 저장하도록 했습니다.
    * 크기가 너무 작은 obstacle의 경우 그냥 outline을 자동 생성하지 않고 직접 값을 설정했습니다.(정밀도를 높여 생성하는 것은 쉽지만 크기가 너무 작다면 비슷한 크기의 aabb로 충분하다고 생각했습니다.)
    * obstacle에 대한 data file을 가지고 있는 mesh인 경우, 생성과 동시에 자동으로 네비게이션 메쉬를 Update합니다.
    * obstacle의 tranform을 조작할 시, obstacle 영역도 함께 재조정됩니다.

⚠️ 발견된 문제
  * obstacle의 크기가 너무 작은 경우, outline이 정교하게 생성되지 않습니다. 수치만 조절하면 해결할 수 있는 문제이지만, 그냥 간단한 형태의 aabb로 대체하기로 했습니다.
  * transform을 조작할 obstacle을 변경할시 프로그램이 중단됩니다. 사이클 문제이므로 코드를 정리하며 수정할 계획입니다.

⚽ 이후 계획
  * obstacle이 다수 배치된 환경에서 경로 탐색을 구현할 예정입니다.

---
# 📅 2024.07.19
📋 진행 사항
  * 3D Obstacle 배치할 때, 지형에 외곽선이 그려지도록 구현했습니다. 다음과 같은 작업들을 통해 정점의 갯수를 줄여 생성되는 삼각형의 수를 줄일 수 있었습니다. 실행 전 미리 수행되는 작업이므로 속도를 크게 고려하지는 않았습니다.
    * 아래와 같은 형태로 Mesh의 하단부에 Ray Casting해 충돌 지점을 검출해 외곽선의 정점을 구성했습니다.
    * 길이 1단위로 Ray와 교차하는 지점을 모두 구한 후, DFS를 이용해 외곽선만을 탐색했습니다.
      
      ![image](https://github.com/user-attachments/assets/555caf8a-e3f4-4413-96d4-8d57e84d33a2)
      
    * 결과는 아래와 같습니다. 아래 사진에서 outline의 총 점점의 갯수는 77개입니다.
      
      ![image](https://github.com/user-attachments/assets/ccd18e59-8bcb-45ca-a7e9-864516c0ee13)
      
  * 외곽선 영역이 Mesh에 너무 가까이 생성되지 않고 약간 확장해 떨어져 생성 될 수 있도록 구현했습니다.
    * 각 정점의 normal을 계산해 외부 방향으로 정점을 확장 이동했습니다. 시계방향 확인을 통해 normal이 영역의 내부를 향한다면 방향을 반대로 뒤집었습니다.

      ```
		vector<Vec3> CNavMeshView::ExpandOutline(const vector<iVec3>& vecOutline, _float fDistance)
		{
			vector<Vec3> vecExpandedOutline;
			_int iSize = vecOutline.size();

			_bool isClockwise = IsClockwise(vecOutline);

			for (_int i = 0; i < iSize; ++i)
     		{
				const iVec3& vPrev = vecOutline[(i - 1 + iSize) % iSize];
				const iVec3& vCurrent = vecOutline[i];
				const iVec3& vNext = vecOutline[(i + 1) % iSize];
				Vec3 vNormal = CalculateNormal(vPrev, vCurrent, vNext);

				if (false == isClockwise)
				{
					vNormal = { -vNormal.x, 0.0f, -vNormal.z };
				}

				Vec3 vExpandedPoint = { (_float)vCurrent.x + vNormal.x * fDistance, (_float)(vCurrent.y), (_float)vCurrent.z + vNormal.z * fDistance };
				vecExpandedOutline.push_back(vExpandedPoint);
      	}

      	return vecExpandedOutline;
      }
      ```
      
      ![image](https://github.com/user-attachments/assets/34341de1-976f-44db-b2fd-be776b3e4e33)
    
  * 외곽선 영역을 확장했을 때, 교차지점이 생기지 않도록 구현했습니다.
    * edge의 교차판정을 통해 두 edge가 교차한다면 사이의 정점들을 모두 제거하고 교차점으로 대체했습니다. 정점의 갯수는 71개입니다.
      ```
      vector<Vec3> CNavMeshView::ProcessIntersections(vector<Vec3>& vecExpandedOutline)
	  {
			vector<Vec3> vecResult;
			_int iSize = vecExpandedOutline.size();

			for (_int i = 0; i < iSize; ++i)
			{
				const Vec3& vP1 = vecExpandedOutline[i];
				const Vec3& vQ1 = vecExpandedOutline[(i + 1) % iSize];

				_bool isIntersected = false;
				for (_int j = i + 2; j < iSize - 1; ++j)
				{
					const Vec3& vP2 = vecExpandedOutline[j % iSize];
					const Vec3& vQ2 = vecExpandedOutline[(j + 1) % iSize];
					Vec3 vIntersection;

					if (true == IntersectSegments(vP1, vQ1, vP2, vQ2, vIntersection))
					{
						vecResult.push_back(vIntersection);
						i = j; // 교차 구간 skip
						isIntersected = true;
						break;
					}
				}

				if (false == isIntersected)
				{
					vecResult.push_back(vP1);
				}
			}

			return vecResult;
	  }
      ```
      ![image](https://github.com/user-attachments/assets/747fea07-aa21-489e-9d48-cf7bbb7e5c3b)

    * 아래는 일부 교차지점이 제거된 모습입니다.
      
      ![image](https://github.com/user-attachments/assets/b0c61277-24ac-48e5-a789-dc1c345f3777)
    
  * 외곽선을 형성하는 정점의 갯수를 줄이고 형태를 단순화 하고자 Douglas Peuker 알고리즘을 적용해 형태를 단순화 했습니다. 최종 정점의 갯수는 15개입니다.
  
  	```
	void CNavMeshView::RamerDouglasPeucker(const vector<Vec3>& vecPointList, _float fEpsilon, OUT vector<Vec3>& vecOut)
	{
		// 가장 멀리 떨어진 선분 탐색
		_float fDmax = 0.0f;
		size_t iIndex = 0;
		size_t iEnd = vecPointList.size() - 1;
	
		for (size_t i = 1; i < iEnd; i++)
		{
			_float fD = PerpendicularDistance(vecPointList[i], vecPointList[0], vecPointList[iEnd]);

			if (fD > fDmax)
			{
				iIndex = i;
				fDmax = fD;
			}
		}

		// fEpsilon보다 fDmax가 크다면
		if (fDmax > fEpsilon)
		{
			// 재귀 수행
			vector<Vec3> vecRecResults1;
			vector<Vec3> vecRecResults2;
			vector<Vec3> vecFirstLine(vecPointList.begin(), vecPointList.begin() + iIndex + 1);
			vector<Vec3> vecLastLine(vecPointList.begin() + iIndex, vecPointList.end());
			RamerDouglasPeucker(vecFirstLine, fEpsilon, vecRecResults1);
			RamerDouglasPeucker(vecLastLine, fEpsilon, vecRecResults2);

			// 최종 리스트
			vecOut.assign(vecRecResults1.begin(), vecRecResults1.end() - 1);
			vecOut.insert(vecOut.end(), vecRecResults2.begin(), vecRecResults2.end());
		}
		else
		{
			vecOut.clear();
			vecOut.push_back(vecPointList[0]);
			vecOut.push_back(vecPointList[iEnd]);
		}
	}
	```

    ![image](https://github.com/user-attachments/assets/17933064-55e1-4b32-988b-75a9e7f44bdb)

  * 최종적으로 삼각형의 갯수를 77개에서 15개로 줄여 생성되는 삼각형의 수와 속도를 향상시킬 수 있었습니다.

⚽ 이후 계획
  * 최종 outline의 정점 데이터를 데이터 파일에 포함할 계획입니다.
  * 삼각형 네비게이션 메쉬에서 경로 탐색을 구현하기 위한 알고리즘 공부할 계획입니다.
  * 현재 obstacle과 교차하는 삼각형을 탐색하기 위해 전체 삼각형을 모두 순회중입니다. 최소한의 영역에서 탐색을 수행할 수 있도록 방안을 고민중입니다.

---
* ~~240719 생각 메모...~~
  * ~~3D Obstacle 배치시, plane terrain에 외곽 Line을 그릴 수 있는 방법을 찾는 중입니다.~~
  * ~~지형을 grid로 분할하고 3D Mesh의 지면 부분에 xz평면에서 raycasting을 해서 mesh와 교차하는 지점을 구한 후, 이를 DFS로 최장거리 탐색을 통해 outline을 계산할 계획입니다.~~
  * ~~계산된 outline을 normal 방향으로 확장하고 Douglas-Peucker 알고리즘을 적용해 외곽선을 근사화해 정점의 갯수를 줄일 계획입니다.~~
  * ~~외곽선 데이터를 파일로 저장한후 obstacle 데이터와 함께 load할 계획입니다.~~

---
# 📅 2024.07.18
📋 진행 사항
  * Dynamic Obstacle 의 생성 및 삭제 시 Cell과의 Intersection을 GPU에서 검출하도록 변경했습니다.
  * AABB to Triangle Intersection의 HLSL코드를 추가했습니다.
    ```
    bool IntersectTriangleAABB(float3 vT0, float3 vT1, float3 vT2, float3 vCenter, float3 vExtents)
	{
 		// ...
    
    	// Axis vA00
    	float3 vA00 = float3(0.0f, -vEdge0.z, vEdge0.y);
    	float fP0 = dot(vV0, vA00);
    	float fP1 = dot(vV1, vA00);
    	float fP2 = dot(vV2, vA00);
    	float fR = vExtents.y * abs(vEdge0.z) + vExtents.z * abs(vEdge0.y);
    	if (max(-Max(fP0, fP1, fP2), Min(fP0, fP1, fP2)) > fR)
     	  return false;

    	// Axis vA01
    	float3 vA01 = float3(0, -vEdge1.z, vEdge1.y);
    	fP0 = dot(vV0, vA01);
    	fP1 = dot(vV1, vA01);
    	fP2 = dot(vV2, vA01);
    	fR = vExtents.y * abs(vEdge1.z) + vExtents.z * abs(vEdge1.y);
    	if (max(-Max(fP0, fP1, fP2), Min(fP0, fP1, fP2)) > fR)
      	  return false;
    
		// Axis vA02
    	// Axis vA10
    	// Axis vA11
    	// Axis vA12
    
    	// ...
    }
    ```
  * 아래와 같이 정상적으로 실행되는 것을 확인했습니다.

    ![FPS_2382-RELEASE2024-07-1910-35-41-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/3d1a9a39-c78f-41a2-b8be-d2d89b513a90)

    * 상수 버퍼의 16byte alignment에 실수가 있어 예상보다 많은 시간을 소모했습니다.
      
⚠️ 발견된 문제
  * 문제라기 보단, cell의 갯수가 적은 현재 상황에서는 하드웨어 환경에 instersection을 CPU에서 구하는 것이 더 효율적인것 같습니다.
    * i9-12900k / RTX3060 OC 에서는 CPU에서, i5-10200H / RTX3060(Laptop) 에서는 GPU에서 더 높은 성능을 보였습니다.

⚽ 이후 계획
  * 3D Obstacle을 배치해 Map을 구성할 계획입니다.
  * 경로 탐색 알고리즘 구현을 위한 사전 지식을 학습할 계획입니다.
  * Map을 Grid로 분할해 Cell Intersection Test 횟수를 더 줄이고 cell의 data를 경량화 할 계획입니다.

---
# 📅 2024.07.17
📋 진행 사항
  * 여러 실험을 거치며 불필요한 코드가 누적돼 16일은 코드를 일부 정리하는 시간을 가졌습니다.
  * 간단하게 xml 형식으로 obstacle의 데이터를 save, load하는 코드를 추가했습니다.(tinyxml2 사용) 아래 이미지는 저장된 데이터 파일입니다.
    ![image](https://github.com/user-attachments/assets/0e7164e7-34e3-4eaf-9e14-86b736cef24f)
  * Dynamic Obstacle의 Navigation Mesh Update 성능 확인을 위한 작업을 진행 중입니다. 계획 중인 테스트 내용은 아래와 같습니다.
    1. 단일 Obstacle에 대해 각도, 위치등을 변경하며 매 프레임 삭제->재생성의 작업을 반복합니다.
    2. 복수 Obstacle에 대해 각도, 위치등을 변경하며 매 프레임 삭제->재생성의 작업을 반복합니다.
  * 1번 테스트의 대략적인 형태는 아래와 같습니다.

    ![FPS_4411-RELEASE2024-07-1810-22-39-ezgif com-video-to-gif-converter (1)](https://github.com/user-attachments/assets/10aa57c4-d36a-475b-8d09-519110b15860)
    
  * 2번 테스트는 아직 복수 Obstacle의 동시 Update를 구현하지 않아 이후에 함께 구현할 계획입니다.

⚠️ 발견된 문제
  * 위 이미지(영상)의 삼각형 cell의 갯수는 많지 않은 데다가(200~220개) 이외에 실행중인 로직이 거의 전무하기 때문에 더 복잡한 상황에서의 결과를 정확히 예측할 수 없습니다.

⚽ 이후 계획
  * 마우스 피킹만으로 obstacle 영역을 지정하는 것은 너무 피로도가 높기 때문에 미뤄왔던 3D Mesh Obstacle을 배치해야 할 것 같습니다.
  * 그전에 Mesh가 불필요하게 많은 삼각형 cell을 생성하지 않도록 방안을 마련할 계획입니다.
  * 현재 obstacle이 생성된 영역의 삼각형 cell을 검출하기 위해 모든 cell을 순회합니다. cell을 늘리기전에 우선 대안을 마련중입니다.

---
# 📅 2024.07.16
📋 진행 사항
  * 2주차 주간 회의 피드백을 통해 남은 6주간 어떤식으로 과제를 해결해 나가야 할지 고민중입니다.
    * 우선 진행중인 Dynamic Obstacle에 대한 Update 기능을 마무리한 후에는, 경로 탐색을 구현할 계획입니다. 최대한 안정적인 성능을 유지할 수 있도록 노력해보려합니다.
    * 이후 2D 상에서 작동하고 있는 시스템을 3D에서도 적용할 수 있도록 고민해보려합니다.

  * 동적으로 삭제되는 obstacle에 대해 주변의 cell만 update하도록 구현했습니다. obstacle이 삭제된 영역의 subset만을 다시 triangulation하는 과정은 대체로 생성과정과 비슷하지만 추가로 고려할 부분이 있었습니다.
    1. subset의 outline에 외부 outline뿐 아니라 내부 hole영역의 outline까지 포함돼, 이를 배제해는 작업이 필요할 것이라 생각했습니다.
      * 따라서 내부의 hole outline을 판별하는 코드를 작성했으나, 사실 이는 전혀 고려할 필요가 없어 취소했습니다.
        * outline의 정보를 보관하는 map자료구조는 outline의 각 point를 key로 사용하기 때문에, 데이터를 삽입 시 좌표에 의해 오름차순 정렬합니다.
        * 어떤 내부 outline 점도 외부 outline의 가장 좌측 점보다 작은 x값을 가질 수 없기 때문에 map의 데이터는 무조건 외부 outline의 점으로 시작합니다.
    3. obstacle의 AABB영역과 교차하는 삼각형 cell을 추출해 subset을 구성할 때, subset이 아래와 같이 설정되는 경우가 있습니다.
      * 이 경우 바로 위 문제 해결의 반례가 될 수 있으며, outline의 단방향 정렬과정에서 외부 outline과 내부의 hole outline이 연결되는 문제가 발생했습니다. 또한 Cell이 충분히 복원되지 않는 문제가 발생했습니다.
      * 해당 지점을 포함하는 삼각형을 추가로 탐색하는 방안도 생각했으나, obstacle의 AABB를 미세하게 확대해 몇 개의 삼각형을 더 포함하는 것으로 간단히 해결할 수 있었습니다. 이러한 방식으로 인해 발생할 수 있는 문제에 대해서는 아직 예상되는 것이 없으나 만에 하나 발생 시 이 부분만 수정할 생각입니다.
        
      ![4](https://github.com/user-attachments/assets/377160c1-2dfd-44df-887a-46935a989a47)

  * Dynamic Obstacle의 Delete 결과는 아래와 같습니다.
    
    ![FPS_61-DEBUG2024-07-1617-54-17-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/94307766-acb6-4ae1-9f25-4416808aa066)

  * obstacle 데이터 save, load 기능 추가 중입니다.(tinyxml2 사용)

⚠️ 발견된 문제
  * obstacle이 겹칠시 생성, 삭제가 제 기능을 하지 않습니다. 특히 삭제 후 복원이 정상적으로 이루어지지 않습니다. 전부터 인지하고 있던 문제이므로, obstacle이 서로 겹치지 않는 상황에서 성능 테스트 후 해결할 계획입니다.

⚽ 이후 계획
  * static obstacle을 다수 배치해 실시간 성능 테스트를 진행해 볼 계획입니다. 이후 위의 문제를 해결해 볼 계획입니다.
  * 동시에 경로 탐색 기능을 추가할 계획입니다. 이 부분은 진행하게 되면 관련 알고리즘을 추가로 공부할 계획입니다.
  
---
2주차 요약 정리 : https://equal-paw-e8f.notion.site/2-ab7058a10821484a9878ed1abdca29ac

---
# 📅 2024.07.15
📋 진행 사항
  * 동적으로 추가되는 obstacle에 대해 주변의 cell만 update하도록 구현했습니다. obstacle이 추가된 영역의 subset만을 triangulation하는 과정은 아래와 같습니다.
    1. 우선 obstacle의 AABB영역과 교차하는 삼각형 cell을 추출해 subset을 구성했습니다.
    2. subset의 cell들 중, subset에 포함되지 않는 cell을 neighbor를 보유한 경우 전체 subset의 outline을 구성할 가능성이 있을 것이라 생각했습니다.
    3. 따라서 해당 edge와 neighbor의 정보를 자료구조에 수집했습니다. 시작 point를 key, 끝 point와 neighbor의 pair를 value로 map에 저장했습니다.
    4. 올바른 triangluation을 위해 map의 edge를 Clockwise로 정렬할 필요가 있었습니다. 모든 edge의 방향은 생성 시 CW로 정렬되어 있기 때문에 edge의 연결 순서만 올바르게 지정하면 됩니다.
    5. map의 key는 edge의 시작점이기 때문에 value의 pair.first에 저장된 끝점을 다시 key로 검색을 계속해 연결된 edge의 list를 얻을 수 있었습니다.
    6. 이렇게 얻은 subset의 edge list로 Delaunay Triangulation을 수행했습니다.

       ![1](https://github.com/user-attachments/assets/d96e319c-4708-48ac-b70e-07fc73e17920)
       
    8. 아래는 코드 예시입니다.
       
        ```
        set<CellData*> setIntersected;
        map<Vec3, pair<Vec3, CellData*>> mapOutlineCells;

        GetIntersectedCells(tObst, setIntersected);	// obstacle과 교차하는 삼각형 cell 추출
        for (auto tCell : setIntersected)
        {
        	for (uint8 i = 0; i < LINE_END; ++i) 	// neighbor가 유효한 edge 추출
       		{
        		if (setIntersected.end() == setIntersected.find(tCell->arrNeighbors[i])) // 해당 edge는 outline
	  	        	{
	    			if (mapOutlineCells.end() == mapOutlineCells.find(tCell->vPoints[i]))	    			
	    			{
	      				mapOutlineCells.emplace(tCell->vPoints[i], pair(tCell->vPoints[(i + 1) % POINT_END], tCell->arrNeighbors[i]));
      				}
        		}
                }
        }
        
        vector<Vec3> vecOutlineCW;	// 시계 방향 정렬
        vecOutlineCW.push_back(mapOutlineCells.begin()->first);

        while (vecOutlineCW.size() < mapOutlineCells.size())
        {
        	auto pair = mapOutlineCells.find(vecOutlineCW.back());
        	if (mapOutlineCells.end() != pair)
        	{        	
        		vecOutlineCW.push_back(pair->second.first);
        	}
        }
  
    9. subset에 대한 triangulation을 수행했다면, 기존 영역의 cell들을 삭제한 후 새로운 subset으로 교체했습니다.
    10. 우선 새로 구성한 subset의 cell들의 neighbor를 전부 지정하고 나면, 자연스럽게 outline을 구성하는 edge의 neighbor는 nullptr일 것이라 생각했습니다.
    11. 이 nullptr상태인 neighbor들은 이전의 map에서 찾을 수 있습니다.
    12. map에서 outline edge의 시작점을 key로 검색해 구한 pair의 second에는 시작점과 끝점이 각각 key, pair.first인 edge의 neighbor가 보관돼 있습니다.
    13. 따라서 outline edge를 포함하는 cell들도 neighbor를 지정해 subset 영역을 기존 영역에 다시 통합할 수 있습니다.

        ![3](https://github.com/user-attachments/assets/0fc43855-0f1a-4037-9ed0-44cbbcd6d7ba)

  * 결과는 아래와 같습니다. 파란색 영역은 새로 계산된 영역입니다.
    
    ![FPS_61-DEBUG2024-07-1610-16-27-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/3a67dfec-7b67-407a-9baa-5ea55d6469bf)


  * 처음부터 모든 것을 고려하고 구현했던 것이 아니기에 발생한 논리적 버그가 많아 많은 시간을 소요했던 것 같습니다.
 
⚠️ 발견된 문제
  * 매우 얇고 긴 obstacle을 생성하려할 때 triangulation을 수행할 수 없습니다. 문제가 될 일은 거의 없을 것 같지만 후에 원인을 파악해보려 합니다.

⚽ 이후 계획
  * 동일하게 obstacle 삭제시에도 일부 영역만을 update할 수 있도록 구현할 계획입니다. 얼핏 생각해서 비슷하게 구현할 수 있을 것 같은데, 방심하지 않으려 합니다.
  
---
# 📅 2024.07.12
📋 진행 사항
  * obstacle의 edge를 하나씩 추가할 경우 이상이 없지만 한번에 edge를 추가한 뒤 triangulation을 하도록 구현하면 hole이 생성되지 않았던 현상을 수정했습니다.
    * hole을 설정하기 위한 내부의 점을 찾는 과정에서 obstacle의 edge를 구성하는 point가 point list에 저장되지 않았습니다.
    * 내부 점을 찾기 이전에 obstacle의 edge를 미리 point list에 추가함으로서 해결할 수 있었습니다.
      ![FPS_60-DEBUG2024-07-1212-12-53-ezgif com-video-to-gif-converter](https://github.com/user-attachments/assets/3bf92443-ff6a-4a06-8463-2aadf66b9483)
  * 동적으로 추가되는 obstacle에 대해 navigation mesh를 수정할 때, obstacle 주위의 cell만 update하도록 구현중입니다.
    * obstacle 정점을 포함하는 AABB영역과 겹치는 삼각형 cell들을 추려 구성한 subset의 outline으로 triangulation을 수행한 후, 다시 원래의 전체 mesh에 합치는 방식으로 구현 중입니다.
    * 간단한 그림으로 표현하면 아래와 같습니다.
      * obstacle영역과 교차하는 cell과 outline을 계산합니다.
        
        ![1](https://github.com/user-attachments/assets/1270518f-3578-455f-a0b6-734f34b79ec8)
      
      * 해당 영역에서 triangulation을 수행합니다.
        
        ![2](https://github.com/user-attachments/assets/708478b4-ee94-4a71-8727-2225dde9a640)
      
      * 결과를 다시 원래의 전체 mesh에 합친 후, neighbor를 연결합니다.
        
        ![3](https://github.com/user-attachments/assets/e91a2886-ccbf-4b21-87f1-4b69e9244cee)
      
      * 현재 어느정도 진행이 되었으나 outline의 시계방향 정렬 등 처리해야할 작업이 남아 구현중입니다.

⚠️ 발견된 문제
  * 불필요한 삼각형이 추가로 생성되는 것으로 보아 정점의 배치 순서가 일정하지 않은 것으로 예상됩니다. outline의 정렬 상태부터 점검해 수정할 계획입니다.
  * 동적으로 navigation mesh를 갱신하는 작업이 생각보다 고려해야 할 사항이 많은 것 같아 시간을 더 투자해 신중하게 작업해 볼 계획입니다.
  * hole 영역을 비워둘 필요가 있는지 재고해보아야 할것 같습니다. 굳이 빈 공간으로 두지 않고 속성을 부여해 obstacle영역임을 구분할 수 있을 것 같아 더 생각해보려합니다.

⚽ 이후 계획
  * 정상적으로 추가가 된다면, obstacle 삭제 시 동적 업데이트 또한 구현할 계획입니다.
  * 방향을 수정하고 시행착오를 겪으면서 비효율적이고 불필요한 코드가 많이 생성돼, obstacle 제거 구현전에 이후 작업의 편의를 위해 구조를 한 번 정리할 계획입니다.
    
---
  * ~~240712 생각 메모...~~
    * ~~obstacle을 추가한 영역에 대해서만 triangle 갱신.~~
      * ~~생각보다 어렵다...~~
    * ~~전체 맵의 edge point 수는 변하지 않음.~~
      * ~~따라서 수정할 부분은 pointcount 이후..?~~
    * ~~obstacle 영역을 포함하는 삼각형을 어떻게든 구하자.~~
    * ~~맵에 grid를 적용?~~
      * ~~각 cell은 어떤 삼각형을 포함하는지 보유하도록 -> obstacle이 위치한 그리드의 삼각형만을 가지고 와서 triangulation.~~
      	* ~~혹은, 최대, 최소 가로 세로 위치의 AABB를 가지고 좀 더 넓은 영역의 삼각형을 triangulation. -> max x, z / mix x, z~~
    * ~~in에는 전체 맵에 대한 초기 point list는 어차피 (사각형이라 가정하면) 8개 고정. 이후부터 obstacle의 point. 이 중 특정 obstacle의 point를 찾아내는건 가능함.~~
    * ~~우선 neighbor들까지 전부 설정된 네비게이션 메쉬를 만들어야함.~~
    * ~~obstacle과 겹치는 삼각형의 좌표를 가지고 와서 triangulation 수행. 결과를 원래 영역에 붙여넣어야함.~~
    * ~~원래 영역의 neighbor와 새로 분할된 영역의 edge를 연결하는 것은 어렵지 않음. edge와 닿아 있는 삼각형들은 neighbor를 다시 돌려주면 되고 나머지는 그냥 자기들끼리 이어주면 된다.~~
    * ~~일단 해보자!~~

---
# 📅 2024.07.11
📋 진행 사항
  * obstacle의 edge가 convex하지 않은 경우 영역의 내부와 외부가 뒤바뀌는 현상을 수정했습니다.
    * triangle library에서는 obstacle 영역을 지정하기 위해 내부의 한 점이 필요한데, 기존에는 이를 간단히 무게 중심으로 설정했습니다.
    * 볼록 다각형이 아닌 경우 형태에 따라 무게 중심의 위치가 다각형의 외부에 있을 수도 있음을 고려하지 못했습니다.(ex. 도넛)
      
      ![FPS_61-DEBUG2024-07-1118-46-08-ezgif com-video-to-gif-converter](https://github.com/ksg1766/Navigation_System/assets/37239034/797257b8-98dc-4644-be74-e6bcd914b5f0)

    * 처음에는 obstacle 영역에 대해 triangulation을 한 번 더 진행하는 방식을 고려했으나, 시간 및 공간 효율이 낮아 다른 방법을 고려하기로 했습니다.
    * 모든 다각형은 삼각형으로 분할 될 수 있기 때문에 반드시 존재할 convex 지점을 찾아 무게 중심을 구하면 해당 점은 다각형의 내부에 위치할 것이라 생각했습니다.
    * 한 점에서 출발한 반직선이 다각형과 짝수 번 교차한다면 점은 외부에, 홀수 번 교차한다면 내부에 존재한다는 사실을 이용했습니다.

      ![image](https://github.com/ksg1766/Navigation_System/assets/37239034/7168ef78-09e5-49c0-be51-f49695824aec)

    * 모든 영역에 대한 triangulation은 수행하지 않을 수 있었습니다. 구현부는 대략 아래와 같습니다.
		```
		for (_int i = 0; i < tObst.numberof; ++i) // numberof : 정점의 갯수
		{
			Vec3 vCenter = Vec3::Zero;

			for(_int j = 0; j < 3; ++j)
			{
				vCenter.x += m_tDT_in.pointlist[tObst.start + (2 * (i + j)) % (2 * tObst.numberof)];
				vCenter.z += m_tDT_in.pointlist[tObst.start + (2 * (i + j) + 1) % (2 * tObst.numberof)];
			}	// 삼각형과 같은 단순한 형태의 다각형은 한 번에 내부 점을 발견할 수 있기 때문에
  				// 한바퀴 돌며 중복으로 정점을 선택할 수 있는 경우에 대한 예외처리는 하지 않았습니다.

			vCenter /= 3.f;

			// RayCast
			_int iCrosses = 0;

			for (_int m = 0; m < tObst.numberof; ++m)
			{
				_int idxX = tObst.start + (2 * m) % (2 * tObst.numberof);
				_int idxZ = tObst.start + (2 * m + 1) % (2 * tObst.numberof);

				_float fSourX = m_tDT_in.pointlist[idxX];
				_float fSourZ = m_tDT_in.pointlist[idxZ];

				idxX = tObst.start + (2 * m + 2) % (2 * tObst.numberof);
				idxZ = tObst.start + (2 * m + 3) % (2 * tObst.numberof);

				_float fDestX = m_tDT_in.pointlist[idxX];
				_float fDestZ = m_tDT_in.pointlist[idxZ];

				if ((fSourX > vCenter.x) != (fDestX > vCenter.x))	// x 좌표 검사
				{
					_float fAtZ = (fDestZ - fSourZ) * (vCenter.x - fSourX) / (fDestX - fSourX) + fSourZ; 

					if (vCenter.z < fAtZ)	// z 좌표 검사
					{
						++iCrosses;
					}
				}
			}

			if (0 < iCrosses % 2)
			{
				tObst.center = vCenter;
				break;
			}
		}
		```
    * 모든 삼각형을 다시 계산하지 않도록 하기 위해, 배치될 obstacle을 포함하는 삼각형만을 계산할 수 있는 방법을 계획 중입니다.

⚠️ 발견된 문제
  * obstacle의 edge를 하나씩 추가할 경우 이상이 없지만 한번에 edge를 추가한 뒤 triangulation을 하면 hole이 생성되지 않습니다.
  * 누락한 부분이 있는지 탐색 중입니다.

⚽ 이후 계획
  * obstacle 정점을 하나씩 배치하는 것이 아닌 3D 오브젝트를 배치할 수 있도록 구현 사항을 변경 계획입니다. 따라서 앞서 언급한 문제를 우선 해결해야 합니다.
  * 모든 삼각형을 다시 계산하지 않도록 하기 위해, 배치될 obstacle을 포함하는 삼각형만을 계산할 수 있는 방법을 구현해 볼 계획입니다.

---
# 📅 2024.07.10
📋 진행 사항
  * 삼각형으로 형성된 지형에 obstacle 영역을 추가했을 때, 해당 영역에 비어있는 hole을 형성하도록(triangulation을 수행하지 않도록) 구현했습니다.
  * triangle library는 Constrainted Delaunay Triangle을 생성하는 라이브러리입니다. 제한된 영역 내에서 triangulation을 수행하기 때문에 영역의 edge를 지정해야 합니다.
  * segment list는 영역의 edge를 결정하는 정점의 index를 저장합니다. 참고할만한 레퍼런스나 예제가 많지 않아 직접 도형을 그려보며 동작 방식을 이해했습니다.
  * 따라서 전체 맵(지형)의 edge를 구성하는 정점의 index를 segment list에 추가한 후, obstacle 영역 edge의 정점 index를 segment list에 추가해 영역을 구분했습니다.
    ```
    // Points
	for (_int i = 0; i < LastIndex_Terrain; ++i)
	{
		m_tDT_in.segmentlist[2 * i]     = i;
		m_tDT_in.segmentlist[2 * i + 1] = i + 1;
	}
	m_tDT_in.segmentlist[2 * LastIndex_Terrain]     = LastIndex_Terrain;
	m_tDT_in.segmentlist[2 * LastIndex_Terrain + 1] = 0;
		
	// Obstacles
	for (_int j = 0; j < m_vecObstacles.size(); ++j)
	{			
		for (_int i = 0; i < m_vecObstacles[j].numberof - 1; ++i)
		{
			m_tDT_in.segmentlist[FirstIndex + 2 * i]     = FirstIndex / 2 + i;
			m_tDT_in.segmentlist[FirstIndex + 2 * i + 1] = FirstIndex / 2 + i + 1;
		}
		m_tDT_in.segmentlist[LastIndex - 1] = LastIndex / 2;
		m_tDT_in.segmentlist[LastIndex]     = FirstIndex / 2;
	}
    ```
  * 위에서 지정한 obstacle영역을 형성하는 segment들의 내부에 hole list의 정점을 위치시키면 해당 영역 내부에서는 triangulation이 이루어지지 않습니다.
    ```
    m_tDT_in.numberofholes = m_vecObstacles.size();
    if (0 < m_tDT_in.numberofholes)
    {
		SAFE_REALLOC(TRI_REAL, m_tDT_in.holelist, m_tDT_in.numberofholes * 2)

		for (_int i = 0; i < m_tDT_in.numberofholes; ++i)
		{
			m_tDT_in.holelist[2 * i + 0] = m_vecObstacles[i].center.x;
			m_tDT_in.holelist[2 * i + 1] = m_vecObstacles[i].center.z;
		}
    }
    ```      
  * 결과는 아래와 같습니다.
    
    ![FPS_61-DEBUG2024-07-1110-29-56-ezgif com-video-to-gif-converter](https://github.com/ksg1766/Navigation_System/assets/37239034/9eea5ec5-55fd-4378-9f63-1d2a3ead0fbe)
      
  * 새로운 정점 및 segment 데이터 등을 추가 할 때, malloc이 아닌 realloc을 통해 매번 모든 정점 데이터를 다시 할당하지 않고 갱신 데이터만 추가하도록 변경했습니다.
    * realloc 할당 실패 시 주소와 데이터가 이동할 가능성이 있으므로 이후 추가로 생각해보려합니다.
  * 모든 지형과 obstacle의 edge 정점 데이터는 단일 point list에 함께 저장되기 때문에, 지형의 edge points를 전부 저장한 뒤에 obstacle의 edge points를 저장하도록 구현했습니다.
    * 이후 과정의 편의성에 영향을 미칠지 꽤 오래 고민해봤는데, 지형데이터를 입력하는 도중에 갑작스러운 변덕으로 obstacle 데이터를 입력하고 다시 지형 데이터를 입력할 일은 없을 것이라 생각했습니다.
    * 지형 데이터가 obstacle 데이터에 의해 분산되면 obstacle 수에 따라 캐시 효율 문제가 발생할 수 있을 것이라 생각했습니다.
    * 물론 입력받은 메모리 주소를 메모리를 정렬할 수도 있지만 어떻게 봐도 효율이 좋지 않은것 같아 데이터 입력 순서는 고정하기로 결정했습니다.

⚠️ 발견된 문제
  * obstacle의 edge가 convex하지 않은 경우 영역의 내부가 잘못 계산 되는 경우가 있습니다.
    * 영역 내부의 중심 좌표로 무게중심 좌표를 사용하고 있는데, convex하지 않은 다각형의 경우 외부에 무게중심 좌표가 계산되는 경우를 고려하지 못했습니다.
    * 다각형을 분할 해서 내부의 점을 찾을 수 있도록 알고리즘을 구현해볼 계획입니다.
      
      ![FPS_61-DEBUG2024-07-1110-35-13-ezgif com-speed](https://github.com/ksg1766/Navigation_System/assets/37239034/77b2232b-b291-4410-9ce5-c2c02c037e93)

⚽ 이후 계획
  * 오늘은 우선 발견된 문제부터 해결하는 것에 집중할 계획입니다.
  * obstacle 영역을 정상적으로 지정할 수 있다면 동적으로 갱신되는 경우에 모든 데이터를 다시 계산하지 않고 obstacle 주변의 영역만 갱신하도록 최적화할 계획입니다.
  * 혹은 A navigation mesh for dynamic environments. (Wouter G. van Toll et Al.) 에서 제안된 방식을 고려해 Voronoi Diagram만을 갱신하는 방법에 대해 고려해볼 계획입니다.

---
# 📅 2024.07.09
📋 진행 사항
  * Delaunay Triangle 의 정점데이터 추가 시 4번째 혹은 그 이후부터 힙 오버플로우 발생하는 현상을 수정했습니다.
    * 분석 및 해결
      1. 삼각형을 구성할 edge의 순서와 각도에 따라, 4번째 정점부터는 도형의 형태에 따라 추가 edge를 생성할 수도 있습니다.
         ![image](https://github.com/ksg1766/Navigation_System/assets/37239034/63962073-9114-4675-b683-589cb604cbf4) fig 1. 5개의 edges ![image](https://github.com/ksg1766/Navigation_System/assets/37239034/e363f2f2-96be-437b-8857-f51127f9c7a2) fig 2. 6개의 edges
      3. 처음에는 단순히 입력 정점 데이터의 잘못된 배열 접근으로 생각 -> 실제로도 코드상의 오류가 있었고 이후 '우연히 연속으로' 둔각으로 정점을 생성하면서 문제가 발생하지 않았습니다.
      4. 해결했다고 착각했으나 이후 같은 문제 계속 발생 -> triangulate 함수에 전달되는 문자열 flag데이터(필요한 output 데이터 설정)에 오류가 있는지 레퍼런스를 전부 확인해 봤지만 오류가 없는 것으로 확인했습니다.
      5. Triangulation을 수행하고 Voronoi Diagram을 생성하는 triangulate 함수가 알고 있던 것보다 더 많은 데이터를 생성할 수 있는 것을 확인했습니다.(ex. 삼각형 영역 list, 삼각형을 생성하지 않을 빈 영역 list 등).
      6. 내부적으로 기존에 이해한 것보다 많은 종류의 데이터를 생성해, 명시적으로 모든 malloc 데이터에 대해 free를 호출하지 않아 발생하는 것으로 확인했습니다.
      7. triangulateio 구조체의 동적 할당 가능한 모든 list데이터에 대해 null check 후 free를 호출할 수 있도록 수정했습니다.
    * Triangle 라이브러리에 대한 이해도 부족과 문제에 대한 1차원적인 접근으로 필요 이상의 많은 시간을 소모했습니다.
   
⚽ 이후 계획
  * 다수 정점과 삼각형 생성에 문제가 없음을 확인했으므로, 지형 Mesh를 이용해 삼각형 데이터 생성 가능한지 확인.
  * 삼각형을 생성하지 않을 빈 영역(obstacle 영역)에 대한 처리 및 obstacle 배치 후 Triangle list 갱신하도록 구현.
  * Triangulation이 진행된 삼각형들에 대해 Neighbor 설정되도록 구현.

---
# 📅 2024.07.08
📋 진행 사항
  * Picking을 통한 Delaunay Triangulation, Voronoi Diagram.([Triangle Library](https://github.com/libigl/triangle.git) 사용 )
![FPS_61-DEBUG2024-07-0910-59-38-ezgif com-speed](https://github.com/ksg1766/Navigation_System/assets/37239034/5ca33f72-e29d-4732-b722-535929e7df46)
 * ~~Boost Library 추가 (polygon/voronoi.hpp 사용 위해)~~ -> Triangle Lib로 대체 후 사용하지 않기로 했습니다.

⚽ 이후 계획
 * Triangle Data Save/Load 및 출력
 * Mesh 통해 얻은 Triangle Data Save/Load 및 출력

⚠️ 발견된 문제
 * ~~특정 시점 정점 추가시 터짐. 원인 찾는 중입니다.~~

---
# 📅 2024.07.05
:clipboard: 진행 사항
  * Slope에 따라 메쉬 추리기.
  * AABB 바운딩 볼륨내에서만 삼각형 생성하도록 제한.
  * 낮은 높이 턱 삼각형은 포함하도록 구현.
![climb](https://github.com/ksg1766/Navigation_System/assets/37239034/3904964d-7b49-4b0a-be59-0efbac5e71e4)

:soccer: 2주차 목표
   * 삼각형 추려내는 작업의 효율이 낮아 목표와 우선 순위를 고려해 Delaunay Triangulation 우선 구현할 계획입니다.
   * Obstacle 정점 추가, 삭제 시 Cell 재구성 되도록 구현 계획입니다.

  * 주요 변경 사항
    * CNavMeshView::BakeNavMesh()
      * climb 가능한 높이 추려내는 작업.
      * m_tNavMeshBoundVolume으로 추려내는 작업.
    * CNavMeshView::SetUpNeighbor()
      * 삼각형 Neighbor 연결함수 추가.

  ⚠️ 발견된 문제
    * Climb 가능한 삼각형 설정 과정에서 불필요한 삼각형 추가.
    * 최적화가 전혀 안돼 속도가 너무 느림.
    * 2주차 목표 진행에 치명적인 지장은 없으므로 병행해서 해결할 계획입니다.

---
  * ~~240705 생각 메모...~~
    * ~~불필요한 삼각형을 형성하는 정점 삭제해야 함. (ex. 급경사 or 천정) 삼각형이 테두리에 위치해 있고(neighbor가 3 미만인 경우) 면적이 무시할만 한 경우 삭제 할것인가~~
    * ~~매우 작은 국소 영역에 대한 처리는 어떻게 할 것인지?~~
      * ~~벽면으로 부터 튀어나온 벽돌의 윗면 등 -> 많은 게임에서 이런 지형을 올라탈 수 있음.~~
      * ~~입력값 이하 크기 삼각형은 아예 삭제 -> 보이지도 않는 돌멩이에 결려 못움직이는 경우 방지. 엔진에서 NavMesh를 Bake하면 작은 식생에도 걸려 못지나가는게 기본. 조금 더 생각해보자.~~
    * ~~최종적으로 불필요한 정점 최대한 삭제 후 Delaunay Triangulation 수행.~~
    * ~~무시해야할 오브젝트는 어떤 경우?~~
    * ~~EmeraldSquare Scene에서 DebugDraw의 Triangle Draw Call 횟수가 너무 많음. -> LINESTRIP이라 BackFace가 없음. 직접 그려야 함.~~
    * ~~slope 관련 작업은 이번주까지...~~

---
# 📅 2024.07.04
:clipboard: 진행 사항
  * 리소스 수집 및 렌더러 제작
    * NVIDIA Emerald Square City Scene, Unity Asset Store 등 활용해 공개된 리소스 수집
     ![FPS_61-RELEASE2024-07-0511-15-22-ezgif com-optimize](https://github.com/ksg1766/Navigation_System/assets/37239034/7838276b-012f-407d-94a9-8cb1936b0502)

    * 리소스 데이터 바이너리 경량화
    * 렌더러 제작
  * 네비게이션 작업 툴 구현 중입니다.
    * Slope에 따라 메쉬 삼각형 추려내어 로드 (테스트 모델)
    * ![FPS_61-RELEASE2024-07-0511-13-04-ezgif com-speed](https://github.com/ksg1766/Navigation_System/assets/37239034/aeee1ca8-6602-424e-b051-9fd0aeda2c5b)
  * Slope로 지형 정점 추려낸 후에 Delaunay Triangulation 이용한 삼각형 집합 구성하도록 구현해 보려 합니다.
---
# 📅 2024.07.03
:clipboard: 진행 사항
  * 논문 분석 및 학습
    * A navigation mesh for dynamic environments, Wouter van Toll et al.
    * Automatic Generated Navigation Mesh Algorithm on 3D Game Scene, Xiang Xu et al.
    * 위 자료 기반으로 Rough하게 아래와 같이 구상했습니다.
      1. 지형 메쉬의 정점 집합과 Delaunay Triangulation 이용해 삼각형 집합 구성.
      2. 생성된 삼각형 집합 이용해 Voronoi Diagram 생성
      3. Diagram 확장해 최초 Navigation Mesh 생성.
      4. 이후 장애물 추가, 삭제 등으로 인한 NavMesh 갱신은 논문 참고해 Voronoi Diagram 수정.
      5. Voronoi Diagram Or 가시성 검사 등 경로 탐색 수행.
    * 리소스 수집 및 렌더러 제작 시작
      * 테스트용 지형 리소스 수집 중입니다.

---
# 📅 2024.07.02
:bulb: 과제 선정 (GI → Navigation System 변경)
 * __Dynamic Navigation Mesh 자동 생성 시스템 & Path Finding 구현.__
 * Recast & Detour와 같은 오픈소스 라이브러리를 사용하지 않고 Dynamic 환경에서 Navigation Mesh 자동 생성 및 빠른 갱신, 경로 탐색 기능 구현을 목표로 합니다.

:soccer: 목표
 * Static Env에서 Nav Mesh 생성
 * Dynamic Env 추가, 삭제에 따른 Nav Mesh 수정
 * 경로 탐색

🔗 자료 수집
 * [A navigation mesh for dynamic environments, Wouter van Toll et al.](https://www.researchgate.net/publication/236903107_A_Navigation_Mesh_for_Dynamic_Environments) (_해당 논문 기반으로 구현할 계획입니다._)
 * [(slideshare)A navigation mesh for dynamic environments, Wouter van Toll et al.](https://www.slideshare.net/slideshow/a-navigation-meshfordynamicenvironments/55973757)
 * [Automatic Generated Navigation Mesh Algorithm on 3D Game Scene, Xiang Xu et al.](https://www.researchgate.net/publication/271406656_Automatic_Generated_Navigation_Mesh_Algorithm_on_3D_Game_Scene)
 * [Incremental Delaunay Triangulation, Dani Lischinski et al.](http://www.karlchenofhell.org/cppswp/lischinski.pdf)
 * [Navigation Meshes for Realistic Multi-Layered Environments, Wouter van Toll et al.](https://webspace.science.uu.nl/~gerae101/pdf/navmesh.pdf)
 * [Hierarchical Path-Finding for Navigation Meshes (HNA*), Nuria Pelechano et al.](https://upcommons.upc.edu/bitstream/handle/2117/98738/Pelechano_HNAstar_prePrint.pdf)
    
---
# 📅 2024.07.01
:bulb: 과제 선정
 * Global Illumination
