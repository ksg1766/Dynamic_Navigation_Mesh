# 📅 2024.07.10
📋 진행 사항
  * 삼각형으로 형성된 Mesh에 Obstacle 영역을 추가했을 때, 해당 영역에 hole을 형성하도록 구현했습니다.
  * triangle library는 Constrainted Delaunay Triangle을 생성하는 라이브러리입니다.
  * segment list는 geometry의 가장자리를 결정하는 정점의 index를 저장합니다. 참고할만한 레퍼런스나 예제가 많지 않아 직접 도형을 그려보며 작동 방식을 숙지했습니다.
  * 따라서 전체 맵(지형)의 가장자리를 구성하는 정점의 index를 segment list에 추가한 후, hole이 될 영역의 index를 segment list에 추가해야합니다.
  * 우선 정점 추가 시 malloc이 아닌 realloc을 통해 매번 모든 정점데이터를 다시 할당하지 않고 갱신 데이터만 추가하도록 변경했습니다.
    * 그러나 realloc 함수도 할당에 실패할 경우 주소와 데이터가 이동할 가능성이 있으므로 이후 추가로 변경할 예정입니다.
  * 작성 중...

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

  * 발견된 문제
    * Climb 가능한 삼각형 설정 과정에서 불필요한 삼각형 추가.
    * 최적화가 전혀 안돼 속도가 너무 느림.
    * 2주차 목표 진행에 치명적인 지장은 없으므로 병행해서 해결할 계획입니다.

---
  * ~~240705 생각...~~
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
