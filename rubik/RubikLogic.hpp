#ifndef __RUBIK_LOGIC_H__
#define __RUBIK_LOGIC_H__
#include <array>
#include <glm/glm.hpp>
#include <iostream>
#include <numeric>
#include <vector>

enum class RubikFaceName
{
  F, ///< front face
  R, ///< right face
  D, ///< Down face
  B, ///< back face
  L, ///< left face
  T, ///< top face
};

using uint = unsigned int;
struct RubikFace {
  RubikFaceName name;
  glm::vec3 n;        ///< Normal (inward)
  glm::vec3 t;        ///< tangent
  glm::vec3 b;        ///< bitangent
  RubikFaceName prev; ///< face in the bitangent direction
  RubikFaceName next; ///< face in the tangent direction

  /// Constructor from a face name
  RubikFace(RubikFaceName name);

private:
  /// Computes the tangent space from the name
  void computeTangentSpace();
};

struct RubikPiece {
  signed char x; ///< the first location coordinate (-1, 0 or 1)
  signed char y; ///< the second location coordinate (-1, 0 or 1)
  signed char z; ///< the third location coordinate (-1, 0 or 1)

  /// Constructor from 3 coordinates
  RubikPiece(signed char x, signed char y, signed char z);

  /// Constructor from a single index (inverse hashing)
  RubikPiece(unsigned int index);

  /// Constructor from a face name and two locations
  RubikPiece(const RubikFace & face, int a, int b);

  /// cast into a single index (direct hashing)
  operator unsigned int() const;
};

struct RubikFacet {
  RubikFaceName face; ///< the face on which the facet lies
  signed char a;      ///< the first location coordinate on the face (-1, 0 or 1)
  signed char b;      ///< the second location coordinate on the face (-1, 0 or 1)

  /// Constructor from a face and 2 coordinates
  RubikFacet(RubikFaceName face, signed char a, signed char b);

  /// Constructor from a single index (inverse hashing)
  RubikFacet(unsigned int index);

  /// cast into a single index (direct hashing)
  operator uint() const;
};

class RubikState {
public:
  /// Default constructor (identity mapping)
  RubikState();

  /// Apply a face rotation
  void applyFaceRotation(const RubikFace & face);

  /**
   * @brief Return the list of pieces that belong to a face
   * @param face the target face
   * @param defaultConfig denotes if the current piece mapping should be disregarded
   * @return the index codes of the wanted pieces
   */
  std::array<uint, 9> piecesOnFace(const RubikFace & face, bool defaultConfig = false);

  /// prints the direct mapping
  void print() const;

private:
  /// Composes the direct facet mapping with a 4-order cycle (left composition)
  void cycle4Facets(uint cycle[4]);

  /// unit test
  static void testCycle();

private:
  using FacetPermutation = std::array<uint, 54>;
  using PiecePermutation = std::array<uint, 27>;
  FacetPermutation m_facetMapping;    ///< permutation of the 54 facets
  FacetPermutation m_invfacetMapping; ///< inverse permutation of the 54 facets
  PiecePermutation m_pieceMapping;    ///< permutation of the 27 pieces
  PiecePermutation m_invpieceMapping; ///< inverse permutation of the 27 pieces
};

#endif // !defined(__RUBIK_LOGIC_H__)
