#include <iostream>
#include <fstream>
#include <stack>
#include <vector>
#include <tuple>
#include <string>
#include <algorithm>

int glob_t = 1;
int glob_s = 0;


int readFile(const std::string& filename,   std::vector< std::vector <int> >& Edges) {
  std::ifstream infile( filename );
  int iLine = 0;
  int x, y;

  while (infile >> x >> y)
  {
    std::vector <int> edgeRow;
    edgeRow.push_back(x);
    edgeRow.push_back(y);
    Edges.push_back(edgeRow);
    iLine++;
  }
  return(0);
}

int createVerts(std::vector< std::vector <int> >& Verts, std::vector< std::vector <int> >& Edges) {
  std::vector <int> vTemp;

  for(int i=0; i<Edges.size(); i++)
  {
    vTemp.push_back(0);
  }

  for(int i=0; i<Edges.size(); i++)
  {
    int v_head_val = Edges[i][0];
    int v_tail_val = Edges[i][1];
    vTemp[v_head_val-1] = 1;
    vTemp[v_tail_val-1] = 1;
  }

  for(int i=0; i<Edges.size(); i++)
  {
    if(vTemp[i]== 1){
      std::vector <int> VertRow;
      // 0. Vertex Value
      VertRow.push_back(i+1);
      // 1. Explored? - used by first pass
      VertRow.push_back(0);
      // 2. Finishing time f(vert_val) = t
      VertRow.push_back(0);
      // 3. Starting index of Edges Adjacency List
      VertRow.push_back(-1);
      // 4. Count of Edges Adjacency List
      VertRow.push_back(-1);
      // 5. Starting index of EdgesRev Adjacency List
      VertRow.push_back(0);
      // 6. Count of EdgesRev Adjacency List
      VertRow.push_back(0);
      // 7. Explored? - used by second pass
      VertRow.push_back(0);
      // 8. leader(v) = finishing time of parent node
      VertRow.push_back(0);
      Verts.push_back(VertRow);
    }else{
      break;
    }
  }

  Verts[0][3] = 0;
  Verts[0][4] = 1;
  for(int i=1; i<Edges.size(); i++)
  {
    int idxVert = Edges[i][0] - 1;
    if(Edges[i][0] == Edges[i-1][0]){
      Verts[idxVert][4] += 1;
    }else{
      Verts[idxVert][3] = i;
      Verts[idxVert][4] = 1;
    }
  }
  return(0);
}

int createEdgesRev(
  std::vector< std::vector <int> >& Verts,
  std::vector< std::vector <int> >& Edges,
  std::vector< std::vector <int> >& EdgesRev) {

  for(int i=0; i<Edges.size(); i++){
    std::vector <int> edgeRevRow;
    edgeRevRow.push_back(Edges[i][1]);
    edgeRevRow.push_back(Edges[i][0]);
    EdgesRev.push_back(edgeRevRow);
  }
  std::sort(EdgesRev.begin(), EdgesRev.end(), [](const std::vector< int >& a, const std::vector< int >& b){ return a[0] < b[0]; } );

  return(0);
}

int addEdgesRevIndicesToVerts(
  std::vector< std::vector <int> >& Verts,
  std::vector< std::vector <int> >& EdgesRev) {

  Verts[0][5] = 0;
  Verts[0][6] = 1;
  for(int i=1; i<EdgesRev.size(); i++)
  {
    int idxVert = EdgesRev[i][0] - 1;
    if(EdgesRev[i][0] == EdgesRev[i-1][0]){
      Verts[idxVert][6] += 1;
    }else{
      Verts[idxVert][5] = i;
      Verts[idxVert][6] = 1;
    }
  }

  return(0);
}

int dfsTopoRcrrRev(std::vector< std::vector <int> >& V, std::vector< std::vector <int> >& E, int idxStartVertex){
  // Book2 pg 50
  // Input: graph G = (V,E) in adjacency list representation, and a vertex s element of V
  // Postcondition: every vertex reachable from s is marked "explored" and has an assigned f-value
  // =========================
  // mark start_vertex as explored
  V[idxStartVertex][1] = 1;
  // find the furthest sink, or return of when I am the sync
  // for each edge (s,v) in s's outgoing adjacency list:
  int rstart = V[idxStartVertex][5];
  int rend   = V[idxStartVertex][5] + V[idxStartVertex][6];
  for(int j=rstart; j<rend; j++){
    int vtail_val = E[j][1];
    // if vtail not yet explored
    if(E[j][0] == V[idxStartVertex][0] and V[vtail_val-1][1] == 0){
      /*
      std::cout << "  trr: " << glob_t << " " << E[j][0] << "=" << V[idxStartVertex][0] << " vtail_val:"  << vtail_val << " "  << V[vtail_val-1][0] << " "  << V[vtail_val-1][1] << std::endl;
      */
      dfsTopoRcrrRev(V,E,vtail_val-1);
    }
  }
  // f(s) = curLabel s's position in order
  V[idxStartVertex][2] = glob_t;
  //print(" pr60: ",V[idxStartVertex][2])
  glob_t += 1;
  return(0);
}

int dfsLeader(
  std::vector< std::vector <int> >& V,
  std::vector< std::vector <int> >& VFT,
  std::vector< std::vector <int> >& E,
  int iV,
  int iVFT){
  // Book2 pg 50
  // Input: graph G = (V,E) in adjacency list representation, and a vertex s element of V
  // Postcondition: every vertex reachable from s is marked "explored" and has an assigned  leader value
  // =========================
  // mark start_vertex as explored
  V[iV][7] = 1;
  V[iV][8] = glob_s;
  VFT[iVFT][7] = 1;
  VFT[iVFT][8] = glob_s;
  // for each edge (s,v) in s's outgoing adjacency list:
  int estart = V[iV][3];
  int eend   = V[iV][3] + V[iV][4];
  /*
  std::cout << "  dfl25: "
            << " iV:" << iV
            << " iVFT:" << iVFT
            << " es ee[" << estart
            << " " << eend << "]"
            << std::endl;
  */
  for(int j=estart; j<eend; j++){
    int vtail_val = E[j][1];
    int next_nd_val = E[j][1];
    int next_nd_idx = E[j][1]-1;
    int next_iVT = V[next_nd_idx][2]-1;
    // if vtail not yet explored
    if(E[j][0] == V[iV][0] and V[vtail_val-1][7] == 0){
      /*
      std::cout << "  dfl50:  "
        << "s:" << glob_s 
        << " i:" << iV 
        << " nn:" << V[iV][0] 
        << " e?:" << V[iV][1] 
        << " ft:" << V[iV][2] 
        << " si:"  << V[iV][3] 
        << " ec:"  << V[iV][4] 
        << " e?:"  << V[iV][7]
        << " ld:"  << V[iV][8]
        << std::endl
        
        << "          E:["  << E[j][0] << " " << vtail_val << "] " << vtail_val << " " << V[vtail_val-1][0] << " "  << V[vtail_val-1][7] 
        
        << " next_nd_val:" << next_nd_val
        << " next_nd_idx:" << next_nd_idx
        << std::endl;
      */
      dfsLeader(V,VFT,E,next_nd_idx,next_iVT);
    }
  }
  //glob_s -= 1;
  return(0);
}


int main() {
  std::vector< std::vector <int> > Edges;
  std::vector< std::vector <int> > Verts;
  std::vector< std::vector <int> > EdgesRev;
  std::vector< std::vector <int> > VSortedByFT;

  std::cout << "C++ Version: " << __cplusplus << "\n";
  //std::string ah = "ah.txt";
  //std::string ah = "v9e11a.txt";
  std::string ah = "2019-03-28-c2ps2-SCC.txt";
  readFile(ah, Edges);

  std::cout<< "Edges:"<< Edges.size() << std::endl;
  for( int i = 0; i< std::min(Edges.size(),20UL); i++ ){
    std::cout<< "["<< Edges[i][0] << " " << Edges[i][1] << "]" << std::endl;
  }
  createVerts(Verts,Edges);
  std::cout<< "Verts:"<< Verts.size() << std::endl;
  for( int i = 0; i< std::min(Verts.size(),20UL); i++ ){
    std::cout<< " " << Verts[i][0];
  }
  std::cout<<std::endl;

  createEdgesRev(Verts,Edges,EdgesRev);
  addEdgesRevIndicesToVerts(Verts,EdgesRev);

  //1st Pass (Book uses TopoSort, which is slighly different)
  for(int i=Verts.size()-1; i>=0; i--){
    if(Verts[i][1] == 0){

      if(i%10000 == 0 || i < 20){
        std::cout << "1pTopcall: " << i << " " << Verts[i][0] << " " << Verts[i][1] << " "  << Verts[i][2] << " "  << Verts[i][3] << " "  << Verts[i][4] << std::endl;
      }
    //dfsTopoRcrr(Verts,Edges,i);
    dfsTopoRcrrRev(Verts,EdgesRev,i);
    }
  }
  for( int i = 0; i< std::min(Verts.size(),20UL); i++ ){
    if(i%10000 == 0 || i < 20){
      std::cout<< "Vert: "<< Verts[i][0] << " " << Verts[i][1] << " "  << Verts[i][2] << " "  << Verts[i][3] << " "  << Verts[i][4] << " "  << Verts[i][5] << " "  << Verts[i][6] <<  " "  << Verts[i][7] << " "  << Verts[i][8] << std::endl;
    }
  }

  //2nd Pass
  //Input: vertices, to be scanned in descending order of Finish Times saved in Verts[i][2] above
  //Output: Leader value

  // Sort in ascending order of finishing times, and use the loop below to do a descending scan
  VSortedByFT = Verts;
  std::sort(VSortedByFT.begin(), VSortedByFT.end(), [](const std::vector< int >& a, const std::vector< int >& b){ return a[2] < b[2]; } );

  for(int i=VSortedByFT.size()-1; i>=0; i--){
    if(VSortedByFT[i][7] == 0){

      glob_s = VSortedByFT[i][2];
      //VSortedByFT[i][7] = 1;
      //VSortedByFT[i][8] = glob_s;

      if(i%10000 == 0 || i < 20){

        std::cout << "2pTopVFT: " 
          << "s:" << glob_s 
          << " i:" << i 
          << " nn:" << VSortedByFT[i][0] 
          << " e?:" << VSortedByFT[i][1] 
          << " ft:" << VSortedByFT[i][2] 
          << " si:"  << VSortedByFT[i][3] 
          << " ec:"  << VSortedByFT[i][4] 
          << " e?:"  << VSortedByFT[i][7] 
          << " ld:"  << VSortedByFT[i][8] 
          << std::endl;
      }

      dfsLeader(Verts,VSortedByFT,Edges,VSortedByFT[i][0]-1,i);
    }
  }

  for( int i = 0; i< std::min(Verts.size(),20UL); i++ ){
    if(i%10000 == 0 || i < 20){
      std::cout<< "Verts: "<< Verts[i][0] << " " << Verts[i][1] << " "  << Verts[i][2] << " "  << Verts[i][3] << " "  << Verts[i][4] << " "  << Verts[i][5] << " "  << Verts[i][6] <<  " "  << Verts[i][7] << " "  << Verts[i][8] << std::endl;
    }
  }

  std::vector<int> SccModes(Verts.size(), 0);
  for( int i = 0; i< Verts.size(); i++ ){
    SccModes[Verts[i][8]-1] += 1;
  }
  std::sort(SccModes.begin(), SccModes.end());
  std::reverse(SccModes.begin(), SccModes.end());

  std::cout << "SccModes: " << std::endl;

  for( int i = 0; i< std::min(SccModes.size(),20UL); i++ ){
    if(i%10000 == 0 || i < 20){
      std::cout << SccModes[i] << " ";
    }
  }
  std::cout << std::endl;

  std::cout << "fin3" << std::endl;
}

