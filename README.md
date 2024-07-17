# 📅 2024.07.16
📋 진행 사항
  * 여러 실험을 거치며 불필요한 코드가 누적돼 16일은 코드를 일부 정리하는 시간을 가졌습니다.
  * 간단하게 xml 형식으로 obstacle의 데이터를 save, load하는 코드를 추가했습니다. 아래 이미지는 저장된 데이터 파일입니다.
    ![image](https://github.com/user-attachments/assets/0e7164e7-34e3-4eaf-9e14-86b736cef24f)
  * Dynamic Obstacle의 Navigation Mesh Update 성능 확인을 위한 작업을 진행 중입니다. 계획 중인 테스트 내용은 아래와 같습니다.
    1. 단일 Obstacle에 대해 각도, 위치등을 변경하며 매 프레임 삭제->재생성의 작업을 반복합니다.
    2. 복수 Obstacle에 대해 각도, 위치등을 변경하며 매 프레임 삭제->재생성의 작업을 반복합니다.
  * 1번 테스트의 대략적인 형태는 아래와 같습니다.

    
    
  * 2번 테스트는 아직 복수 Obstacle의 동시 Update를 구현하지 않아 이후에 함께 구현할 계획입니다.

⚠️ 발견된 문제
  * 위 이미지(영상)의 삼각형 cell의 갯수는 많지 않은 데다가(###개) 이외에 실행중인 로직이 거의 전무하기 때문에 더 복잡한 상황에서의 결과를 정확히 예측할 수 없습니다.

⚽ 이후 계획
  * 마우스 피킹만으로 obstacle 영역을 지정하는 것은 너무 피로도가 높기 때문에 미뤄왔던 3D Mesh Obstacle을 배치해야 할 것 같습니다.
  * 그전에 Mesh가 불필요하게 많은 삼각형 cell을 생성하지 않도록 방안을 마련할 계획입니다.
  * 현재 obstacle이 생성된 영역의 삼각형 cell을 검출하기 위해 모든 cell을 순회합니다. cell을 늘리기전에 우선 대안을 마련하고자 합니다.

---
# 📅 2024.07.15
📋 진행 사항
  * 2주차 주간 회의 피드백을 통해 남은 6주간 어떤식으로 과제를 해결해 나가야 할지 고민중입니다.
    * 우선 진행중인 Dynamic Obstacle에 대한 Update 기능을 마무리한 후에는, 경로 탐색을 구현할 예정입니다. 최대한 안정적인 성능을 유지할 수 있도록 노력해보려합니다.
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
  * 동일하게 obstacle 삭제시에도 일부 영역만을 update할 수 있도록 구현할 예정입니다. 얼핏 생각해서 비슷하게 구현할 수 있을 것 같은데, 방심하지 않으려 합니다.
  
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
  * obstacle 정점을 하나씩 배치하는 것이 아닌 3D 오브젝트를 배치할 수 있도록 구현 사항을 변경 예정입니다. 따라서 앞서 언급한 문제를 우선 해결해야 합니다.
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
   * Obstacle 정점 추가, 삭제 시 Cell 재구성 되도록 구현 예정입니다.

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
 * [A navigation mesh for dynamic environments, Wouter van Toll et al.](https://www.researchgate.net/publication/236903107_A_Navigation_Mesh_for_Dynamic_Environments) (_해당 논문 기반으로 구현할 예정입니다._)
 * [(slideshare)A navigation mesh for dynamic environments, Wouter van Toll et al.](https://www.slideshare.net/slideshow/a-navigation-meshfordynamicenvironments/55973757)
 * [Automatic Generated Navigation Mesh Algorithm on 3D Game Scene, Xiang Xu et al.](https://www.researchgate.net/publication/271406656_Automatic_Generated_Navigation_Mesh_Algorithm_on_3D_Game_Scene)
 * [Incremental Delaunay Triangulation, Dani Lischinski et al.](http://www.karlchenofhell.org/cppswp/lischinski.pdf)
 * [Navigation Meshes for Realistic Multi-Layered Environments, Wouter van Toll et al.](https://webspace.science.uu.nl/~gerae101/pdf/navmesh.pdf)
 * [Hierarchical Path-Finding for Navigation Meshes (HNA*), Nuria Pelechano et al.](https://upcommons.upc.edu/bitstream/handle/2117/98738/Pelechano_HNAstar_prePrint.pdf)
    
---
# 📅 2024.07.01
:bulb: 과제 선정
 * Global Illumination
