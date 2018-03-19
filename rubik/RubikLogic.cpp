#include "RubikLogic.hpp"

RubikFace::RubikFace(RubikFaceName name) : name(name), prev(RubikFaceName((uint(name) + 5) % 6)), next(RubikFaceName((uint(name) + 1) % 6))
{
  computeTangentSpace();
}

void RubikFace::computeTangentSpace()
{
  switch (name) {
  case RubikFaceName::F:
    t = {1, 0, 0};
    b = {0, 1, 0};
    n = {0, 0, 1};
    assert(prev == RubikFaceName::T);
    assert(next == RubikFaceName::R);
    break;
  case RubikFaceName::R:
    t = {0, -1, 0};
    b = {0, 0, 1};
    n = {-1, 0, 0};
    assert(prev == RubikFaceName::F);
    assert(next == RubikFaceName::D);
    break;
  case RubikFaceName::D:
    t = {0, 0, 1};
    b = {1, 0, 0};
    n = {0, 1, 0};
    assert(prev == RubikFaceName::R);
    assert(next == RubikFaceName::B);
    break;
  case RubikFaceName::B:
    t = {-1, 0, 0};
    b = {0, 1, 0};
    n = {0, 0, -1};
    assert(prev == RubikFaceName::D);
    assert(next == RubikFaceName::L);
    break;
  case RubikFaceName::L:
    t = {0, 1, 0};
    b = {0, 0, 1};
    n = {1, 0, 0};
    assert(prev == RubikFaceName::B);
    assert(next == RubikFaceName::T);
    break;
  case RubikFaceName::T:
    t = {0, 0, -1};
    b = {1, 0, 0};
    n = {0, -1, 0};
    assert(prev == RubikFaceName::L);
    assert(next == RubikFaceName::F);
    break;
  default:
    break;
  }
}

RubikPiece::RubikPiece(signed char x, signed char y, signed char z) : x(x), y(y), z(z) {}

RubikPiece::RubikPiece(unsigned int index) : x(index / 9 % 3 - 1), y(index / 3 % 3 - 1), z(index % 3 - 1) {}

RubikPiece::RubikPiece(const RubikFace & face, int a, int b)
{
  glm::vec3 code = float(a) * face.t + float(b) * face.b - face.n;
  x = code.x;
  y = code.y;
  z = code.z;
}

RubikPiece::operator unsigned int() const
{
  return (x + 1) * 9 + (y + 1) * 3 + (z + 1);
}

RubikFacet::RubikFacet(RubikFaceName face, signed char a, signed char b) : face(face), a(a), b(b) {}

RubikFacet::RubikFacet(unsigned int index) : face(static_cast<RubikFaceName>(index / 9)), a(index / 3 % 3 - 1), b(index % 3 - 1) {}

RubikFacet::operator uint() const
{
  return static_cast<uint>(face) * 9 + (a + 1) * 3 + (b + 1);
}

RubikState::RubikState()
{
  std::iota(m_facetMapping.begin(), m_facetMapping.end(), 0);       // identity (0...53)
  std::iota(m_invfacetMapping.begin(), m_invfacetMapping.end(), 0); // identity (0...53)
  std::iota(m_pieceMapping.begin(), m_pieceMapping.end(), 0);       // identity (0...26)
  std::iota(m_invpieceMapping.begin(), m_invpieceMapping.end(), 0); // identity
}

void RubikState::applyFaceRotation(const RubikFace & face)
{
  // update the facet permutation
  RubikFace next(face.next);
  RubikFace prev(face.prev);
  std::array<uint, 4> cycle;
  cycle = {{RubikFacet(face.name, -1, -1), RubikFacet(face.name, 1, -1), RubikFacet(face.name, 1, 1), RubikFacet(face.name, -1, 1)}};
  cycle4Facets(cycle.data());
  cycle = {{RubikFacet(face.name, 0, -1), RubikFacet(face.name, 1, 0), RubikFacet(face.name, 0, 1), RubikFacet(face.name, -1, 0)}};
  cycle4Facets(cycle.data());
  cycle = {{RubikFacet(face.next, 0, 1), RubikFacet(face.prev, 1, 0), RubikFacet(prev.prev, 0, -1), RubikFacet(next.next, -1, 0)}};
  cycle4Facets(cycle.data());
  cycle = {{RubikFacet(face.next, -1, 1), RubikFacet(face.prev, 1, -1), RubikFacet(prev.prev, 1, -1), RubikFacet(next.next, -1, 1)}};
  cycle4Facets(cycle.data());
  cycle = {{RubikFacet(face.next, 1, 1), RubikFacet(face.prev, 1, 1), RubikFacet(prev.prev, -1, -1), RubikFacet(next.next, -1, -1)}};
  cycle4Facets(cycle.data());
  // update the piece permutation
  std::array<uint, 9> pieces = piecesOnFace(face, true);
  std::vector<uint> invpieces;
  for (uint a = 0; a < 3; ++a) {
    for (uint b = 0; b < 3; ++b) {
      uint p1 = pieces[a * 3 + b];       // source piece
      uint p2 = pieces[(2 - b) * 3 + a]; // target piece (clockwise rotation of p1);
      m_pieceMapping[m_invpieceMapping[p2]] = p1;
      invpieces.push_back(m_invpieceMapping[p2]);
    }
  }
  // update the inverse mapping
  for (uint invp : invpieces) {
    m_invpieceMapping[m_pieceMapping[invp]] = invp;
  }
  for (uint p = 0; p < m_pieceMapping.size(); ++p) {
    assert(p == m_invpieceMapping[m_pieceMapping[p]]);
    assert(p == m_pieceMapping[m_invpieceMapping[p]]);
  }
}

void RubikState::cycle4Facets(uint cycle[])
{
  uint x1 = cycle[0];
  uint x2 = cycle[1];
  uint x3 = cycle[2];
  uint x4 = cycle[3];
  m_facetMapping[m_invfacetMapping[x1]] = x2;
  m_facetMapping[m_invfacetMapping[x2]] = x3;
  m_facetMapping[m_invfacetMapping[x3]] = x4;
  m_facetMapping[m_invfacetMapping[x4]] = x1;
  for (auto y : {m_invfacetMapping[x1], m_invfacetMapping[x2], m_invfacetMapping[x3], m_invfacetMapping[x4]}) {
    m_invfacetMapping[m_facetMapping[y]] = y;
  }
}

void RubikState::print() const
{
  uint cnt = 0;
  std::cout << "facets: [";
  for (auto n : m_facetMapping) {
    if (n != cnt) {
      std::cout << cnt << "->" << n << ' ';
    }
    cnt++;
  }
  std::cout << ']' << std::endl;
  cnt = 0;
  std::cout << "pieces: [";
  for (auto n : m_pieceMapping) {
    if (n != cnt) {
      std::cout << cnt << "->" << n << ' ';
    }
    cnt++;
  }
  std::cout << ']' << std::endl;
}

void RubikState::testCycle()
{
  RubikState s;
  s.print();
  uint twoCycles[] = {
      1, 7, 9, 10, 1, 4, 2, 10,
  };
  uint * cycle = twoCycles;
  s.cycle4Facets(cycle);
  assert(s.m_facetMapping[1] == 7);
  assert(s.m_facetMapping[7] == 9);
  assert(s.m_facetMapping[9] == 10);
  assert(s.m_facetMapping[10] == 1);
  s.print();
  cycle = twoCycles + 4;
  s.cycle4Facets(cycle);
  assert(s.m_facetMapping[1] == 7);
  assert(s.m_facetMapping[4] == 2);
  assert(s.m_facetMapping[2] == 10);
  assert(s.m_facetMapping[7] == 9);
  assert(s.m_facetMapping[9] == 1);
  assert(s.m_facetMapping[10] == 4);
  s.print();
}

std::array<uint, 9> RubikState::piecesOnFace(const RubikFace & face, bool defaultConfig)
{
  std::array<uint, 9> pieces;
  uint k = 0;
  for (int a = -1; a <= 1; a++) {
    for (int b = -1; b <= 1; b++) {
      uint piece = RubikPiece(face, a, b);
      if (not defaultConfig) {
        piece = m_invpieceMapping[piece];
      }
      pieces[k++] = piece;
    }
  }
  return pieces;
}
