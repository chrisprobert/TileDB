/**
 * @file   array_metadata.h
 *
 * @section LICENSE
 *
 * The MIT License
 *
 * @copyright Copyright (c) 2017 TileDB, Inc.
 * @copyright Copyright (c) 2016 MIT and Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 * This file defines class ArrayMetadata.
 */

#ifndef TILEDB_ARRAY_METADATA_H
#define TILEDB_ARRAY_METADATA_H

#include <string>
#include <typeinfo>
#include <vector>

#include "array_type.h"
#include "attribute.h"
#include "buffer.h"
#include "compressor.h"
#include "constants.h"
#include "datatype.h"
#include "dimension.h"
#include "hyperspace.h"
#include "layout.h"
#include "status.h"
#include "uri.h"

namespace tiledb {

/** Specifies the array metadata. */
class ArrayMetadata {
 public:
  /* ********************************* */
  /*     CONSTRUCTORS & DESTRUCTORS    */
  /* ********************************* */

  /** Constructor. */
  ArrayMetadata();

  /**
   * Constructor. Clones the input.
   *
   * @param array_metadata The array metadata to copy.
   */
  explicit ArrayMetadata(const ArrayMetadata* array_metadata);

  /**
   * Constructor.
   *
   * @param uri The array uri.
   */
  explicit ArrayMetadata(const URI& uri);

  /** Destructor. */
  ~ArrayMetadata();

  /* ********************************* */
  /*               API                 */
  /* ********************************* */

  /** Returns the array uri. */
  const URI& array_uri() const;

  /** Returns the array type. */
  ArrayType array_type() const;

  /** Returns a constant pointer to the selected attribute (NULL if error). */
  const Attribute* attribute(unsigned int id) const;

  /** Returns the name of the attribute with the input id. */
  const std::string& attribute_name(unsigned id) const;

  /**
   * Returns the id of the input attribute.
   *
   * @param attribute The attribute name whose id will be retrieved.
   * @param The attribute id if the input attribute exists.
   * @return Status
   *
   */
  Status attribute_id(const std::string& attribute, unsigned int* id) const;

  /**
   * Returns the attribute names, plus the coordinates spacial name
   * in the back.
   */
  std::vector<std::string> attribute_names() const;

  /** Returns the number of attributes. */
  unsigned int attribute_num() const;

  /** Returns the attributes. */
  const std::vector<Attribute*>& Attributes() const;

  /** Returns the capacity. */
  uint64_t capacity() const;

  /**
   * Returns the number of cells per tile. Meaningful only for the dense case.
   */
  uint64_t cell_num_per_tile() const;

  /** Returns the cell order. */
  Layout cell_order() const;

  /** Returns the size of cell on the input attribute. */
  uint64_t cell_size(unsigned int attribute_id) const;

  /** Returns the number of values per cell of the input attribute. */
  unsigned int cell_val_num(unsigned int attribute_id) const;

  /**
   * Checks the correctness of the array metadata.
   *
   * @return Status
   */
  Status check() const;

  /** Returns the compression type of the attribute with the input id. */
  Compressor compression(unsigned int attribute_id) const;

  /** Return the compression level of the attribute with the input id. */
  int compression_level(unsigned int attribute_id) const;

  /** Returns the compressor of the coordinates. */
  Compressor coords_compression() const;

  /** Returns the compression level of the coordinates. */
  int coords_compression_level() const;

  /** Returns the coordinates size. */
  uint64_t coords_size() const;

  /** Returns the type of the coordinates. */
  Datatype coords_type() const;

  /** True if the array is dense. */
  bool dense() const;

  /** Returns the i-th dimension. */
  const Dimension* dimension(unsigned int i) const;

  /** Returns the number of dimensions. */
  unsigned int dim_num() const;

  /** Returns the domain. */
  const void* domain() const;

  /** Dumps the array metadata in ASCII format in the selected output. */
  void dump(FILE* out) const;

  /**
   * Gets the ids of the input attributes.
   *
   * @param attributes The name of the attributes whose ids will be retrieved.
   * @param attribute_ids The ids that are retrieved by the function.
   * @return Status
   */
  Status get_attribute_ids(
      const std::vector<std::string>& attributes,
      std::vector<unsigned int>& attribute_ids) const;

  /**
   * Returns true if the input range is contained fully in a single
   * column of tiles.
   */
  bool is_contained_in_tile_slab_col(const void* range) const;

  /**
   * Returns true if the input range is contained fully in a single
   * column of tiles.
   *
   * @tparam T The coordinates type.
   * @param range The input range.
   * @return True if the input range is contained fully in a single
   *     column of tiles.
   */
  template <class T>
  bool is_contained_in_tile_slab_col(const T* range) const;

  /**
   * Returns true if the input range is contained fully in a single
   * row of tiles.
   */
  bool is_contained_in_tile_slab_row(const void* range) const;

  /**
   * Returns true if the input range is contained fully in a single
   * row of tiles.
   *
   * @tparam T The coordinates type.
   * @param range The input range.
   * @return True if the input range is contained fully in a single
   *     row of tiles.
   */
  template <class T>
  bool is_contained_in_tile_slab_row(const T* range) const;

  /**
   * Serializes the array metadata object into a buffer.
   *
   * @param buff The buffer the array metadata is serialized into.
   * @return Status
   */
  Status serialize(Buffer* buff) const;

  /**
   * Returns the type of overlap of the input subarrays.
   *
   * @tparam T The types of the subarrays.
   * @param subarray_a The first input subarray.
   * @param subarray_b The second input subarray.
   * @param overlap_subarray The overlap area between *subarray_a* and
   *     *subarray_b*.
   * @return The type of overlap, which can be one of the following:
   *    - 0: No overlap
   *    - 1: *subarray_a* fully covers *subarray_b*
   *    - 2: Partial overlap (non-contig)
   *    - 3: Partial overlap (contig)
   */
  template <class T>
  unsigned int subarray_overlap(
      const T* subarray_a, const T* subarray_b, T* overlap_subarray) const;

  /** Returns the tile extents. */
  const void* tile_extents() const;

  /**
   * Returns the number of tiles in the array domain (applicable only to dense
   * arrays).
   */
  uint64_t tile_num() const;

  /**
   * Returns the number of tiles in the array domain (applicable only to dense
   * arrays).
   *
   * @tparam T The coordinates type.
   * @return The number of tiles.
   */
  template <class T>
  uint64_t tile_num() const;

  /**
   * Returns the number of tiles overlapping with the input range
   * (applicable only to dense arrays).
   */
  uint64_t tile_num(const void* range) const;

  /**
   * Returns the number of tiles in the input domain (applicable only to dense
   * arrays).
   *
   * @tparam T The coordinates type.
   * @param domain The input domain.
   * @return The number of tiles.
   */
  template <class T>
  uint64_t tile_num(const T* domain) const;

  /** Returns the tile order. */
  Layout tile_order() const;

  /** Return the number of cells in a column tile slab of an input subarray. */
  uint64_t tile_slab_col_cell_num(const void* subarray) const;

  /** Return the number of cells in a row tile slab of an input subarray. */
  uint64_t tile_slab_row_cell_num(const void* subarray) const;

  /** Returns the type of the i-th attribute, or NULL if 'i' is invalid. */
  Datatype type(unsigned int i) const;

  /** Returns the type size of the i-th attribute. */
  uint64_t type_size(unsigned int i) const;

  /** Returns *true* if the indicated attribute has variable-sized values. */
  bool var_size(unsigned int attribute_id) const;

  /** Adds an attribute, cloning the input. */
  void add_attribute(const Attribute* attr);

  /**
   * It assigns values to the members of the object from the input buffer.
   *
   * @param buff The binary representation of the object to read from.
   * @return Status
   */
  Status deserialize(ConstBuffer* buff);

  /**
   * Initializes the ArrayMetadata object. It also performs a check to see if
   * all the member attributes have been properly set.
   *
   * @return Status
   */
  Status init();

  /** Sets the array type. */
  void set_array_type(ArrayType array_type);

  /** Sets the tile capacity. */
  void set_capacity(uint64_t capacity);

  /** Sets the cell order. */
  void set_cell_order(Layout cell_order);

  /** Sets the hyperspace. */
  void set_hyperspace(Hyperspace* hyperspace);

  /** Sets the tile order. */
  void set_tile_order(Layout tile_order);

  /**
   * Checks the cell order of the input coordinates. Note that, in the presence
   * of a regular tile grid, this function assumes that the cells are in the
   * same regular tile.
   *
   * @tparam T The coordinates type.
   * @param coords_a The first input coordinates.
   * @param coords_b The second input coordinates.
   * @return One of the following:
   *    - -1 if the first coordinates precede the second
   *    -  0 if the two coordinates are identical
   *    - +1 if the first coordinates succeed the second
   */
  template <class T>
  int cell_order_cmp(const T* coords_a, const T* coords_b) const;

  /**
   * Expands the input domain such that it coincides with the boundaries of
   * the array's regular tiles (i.e., it maps it on the regular tile grid).
   * If the array has no regular tile grid, the function does not do anything.
   *
   * @param domain The domain to be expanded.
   * @return void
   */
  void expand_domain(void* domain) const;

  /**
   * Expands the input domain such that it coincides with the boundaries of
   * the array's regular tiles (i.e., it maps it on the regular tile grid).
   * If the array has no regular tile grid, the function does not do anything.
   *
   * @tparam The domain type.
   * @param domain The domain to be expanded.
   * @return void
   */
  template <class T>
  void expand_domain(T* domain) const;

  /**
   * Returns the position of the input coordinates inside its corresponding
   * tile, based on the array cell order. Applicable only to **dense** arrays.
   *
   * @tparam T The coordinates type.
   * @param coords The input coordindates, which are expressed as global
   *     coordinates in the array domain.
   * @pos The position of the cell coordinates in the array cell order
   *     within its corresponding tile.
   * @return Status
   *
   */
  template <class T>
  Status get_cell_pos(const T* coords, uint64_t* pos) const;

  /**
   * Retrieves the next coordinates along the array cell order within a given
   * domain (desregarding whether the domain is split into tiles or not).
   * Applicable only to **dense** arrays.
   *
   * @tparam T The coordinates type.
   * @param domain The targeted domain.
   * @param cell_coords The input cell coordinates, which the function modifies
   *     to store the next coordinates at termination.
   * @param coords_retrieved Will store true if the retrieved coordinates are
   *     inside the domain, and false otherwise.
   * @return void
   */
  template <class T>
  void get_next_cell_coords(
      const T* domain, T* cell_coords, bool& coords_retrieved) const;

  /**
   * Retrieves the next tile coordinates along the array tile order within a
   * given tile domain. Applicable only to **dense** arrays.
   *
   * @tparam T The coordinates type.
   * @param domain The targeted domain.
   * @param tile_coords The input tile coordinates, which the function modifies
   *     to store the next tile coordinates at termination.
   * @return void
   */
  template <class T>
  void get_next_tile_coords(const T* domain, T* tile_coords) const;

  /**
   * Retrieves the previous coordinates along the array cell order within a
   * given domain (desregarding whether the domain is split into tiles or not).
   * Applicable only to **dense** arrays.
   *
   * @tparam T The coordinates type.
   * @param domain The targeted domain.
   * @param cell_coords The input cell coordinates, which the function modifies
   *     to store the previous coordinates at termination.
   * @return void
   */
  template <class T>
  void get_previous_cell_coords(const T* domain, T* cell_coords) const;

  /**
   * Gets a subarray of tile coordinates for the input (cell) subarray
   * over the input array domain. Retrieves also the tile domain of
   * the array..
   *
   * @tparam T The domain type.
   * @param subarray The input (cell) subarray.
   * @param tile_domain The array tile domain to be retrieved.
   * @param subarray_in_tile_domain The output (tile) subarray.
   * @return void
   */
  template <class T>
  void get_subarray_tile_domain(
      const T* subarray, T* tile_domain, T* subarray_in_tile_domain) const;

  /**
   * Returns the tile position along the array tile order within the input
   * domain. Applicable only to **dense** arrays.
   *
   * @tparam T The domain type.
   * @param tile_coords The tile coordinates.
   * @return The tile position of *tile_coords* along the tile order of the
   *     array inside the array domain, or TILEDB_AS_ERR on error.
   */
  template <class T>
  uint64_t get_tile_pos(const T* tile_coords) const;

  /**
   * Returns the tile position along the array tile order within the input
   * domain. Applicable only to **dense** arrays.
   *
   * @tparam T The domain type.
   * @param domain The input domain, which is a cell domain partitioned into
   *     regular tiles in the same manner as that of the array domain (however
   *     *domain* may be a sub-domain of the array domain).
   * @param tile_coords The tile coordinates.
   * @return The tile position of *tile_coords* along the tile order of the
   *     array inside the input domain, or TILEDB_AS_ERR on error.
   */
  template <class T>
  uint64_t get_tile_pos(const T* domain, const T* tile_coords) const;

  /**
   * Gets the tile subarray for the input tile coordinates.
   *
   * @tparam T The coordinates type.
   * @param tile_coords The input tile coordinates.
   * @param tile_subarray The output tile subarray.
   * @return void.
   */
  template <class T>
  void get_tile_subarray(const T* tile_coords, T* tile_subarray) const;

  /** Returns the array hyperspace. */
  const Hyperspace* hyperspace() const;

  /**
   * Checks the order of the input coordinates. First the tile order is checked
   * (which, in case of non-regular tiles, is always the same), breaking the
   * tie by checking the cell order.
   *
   * @tparam T The coordinates type.
   * @param coords_a The first input coordinates.
   * @param coords_b The second input coordinates.
   * @return One of the following:
   *    - -1 if the first coordinates precede the second
   *    -  0 if the two coordinates are identical
   *    - +1 if the first coordinates succeed the second
   */
  template <class T>
  int tile_cell_order_cmp(const T* coords_a, const T* coords_b) const;

  /**
   * Returns the id of the tile the input coordinates fall into.
   *
   * @tparam T The coordinates type.
   * @param cell_coords The input coordinates.
   * @return The computed tile id.
   */
  template <class T>
  uint64_t tile_id(const T* cell_coords) const;

  /**
   * Checks the tile order of the input coordinates.
   *
   * @tparam T The coordinates type.
   * @param coords_a The first input coordinates.
   * @param coords_b The second input coordinates.
   * @return One of the following:
   *    - -1 if the first coordinates precede the second on the tile order
   *    -  0 if the two coordinates have the same tile order
   *    - +1 if the first coordinates succeed the second on the tile order
   */
  template <class T>
  int tile_order_cmp(const T* coords_a, const T* coords_b) const;

  /* ********************************* */
  /*        AUXILIARY ATTRIBUTES       */
  /* ********************************* */

  /**
   * Auxiliary attribute used in the computation of tile ids, in order to avoid
   * repeated allocations and deallocations that impact performance.
   */
  void* tile_coords_aux_;

 private:
  /* ********************************* */
  /*         PRIVATE ATTRIBUTES        */
  /* ********************************* */

  /** The array name. */
  URI array_uri_;

  /** The array type. */
  ArrayType array_type_;

  /** The number of attributes. */
  unsigned int attribute_num_;

  /** The array attributes. */
  std::vector<Attribute*> attributes_;
  /**
   * The tile capacity for the case of sparse fragments.
   */
  uint64_t capacity_;

  /** The number of cells per tile. Meaningful only for the **dense** case. */
  uint64_t cell_num_per_tile_;

  /**
   * The cell order. It can be one of the following:
   *    - TILEDB_ROW_MAJOR
   *    - TILEDB_COL_MAJOR
   */
  Layout cell_order_;

  /** Stores the size of every attribute (plus coordinates in the end). */
  std::vector<uint64_t> cell_sizes_;

  /** The coordinates compression type. */
  Compressor coords_compression_;

  /** The coordinates compression level. */
  int coords_compression_level_;

  /** The size (in bytes) of the coordinates. */
  uint64_t coords_size_;

  /**
   * The array domain. It should contain one [lower, upper] pair per dimension.
   * The type of the values stored in this buffer should match the coordinates
   * type.
   */
  void* domain_;

  /** The array hyperspace. */
  Hyperspace* hyperspace_;

  /**
   * The array domain. It should contain one [lower, upper] pair per dimension.
   * The type of the values stored in this buffer should match the coordinates
   * type.
   */
  void* tile_domain_;

  /**
   * The tile extents (only applicable to regular tiles). There should be one
   * value for each dimension. The type of the values stored in this buffer
   * should match the coordinates type. If it is NULL, then it means that the
   * array has irregular tiles (and, hence, it is sparse).
   */
  void* tile_extents_;

  /**
   * Offsets for calculating tile positions and ids for the column-major
   * tile order.
   */
  std::vector<uint64_t> tile_offsets_col_;

  /**
   * Offsets for calculating tile positions and ids for the row-major
   * tile order.
   */
  std::vector<uint64_t> tile_offsets_row_;

  /**
   * The tile order. It can be one of the following:
   *    - TILEDB_ROW_MAJOR
   *    - TILEDB_COL_MAJOR
   */
  Layout tile_order_;

  /* ********************************* */
  /*           PRIVATE METHODS         */
  /* ********************************* */

  /** Clears all members. Use with caution! */
  void clear();

  /**
   * Compute the number of cells per tile. Meaningful only for the **dense**
   * case.
   *
   * @return void
   */
  void compute_cell_num_per_tile();

  /**
   * Compute the number of cells per tile. Meaningful only for the **dense**
   * case.
   *
   * @tparam T The coordinates type.
   * @return void
   */
  template <class T>
  void compute_cell_num_per_tile();

  /** Computes and returns the size of an attribute (or coordinates). */
  uint64_t compute_cell_size(unsigned int attribute_id) const;

  /**
   * Computes the tile domain. Applicable only to arrays with regular tiles.
   *
   * @return void
   */
  void compute_tile_domain();

  /**
   * Computes tile offsets neccessary when computing tile positions and ids.
   *
   * @return void
   */
  void compute_tile_offsets();

  /**
   * Computes tile offsets neccessary when computing tile positions and ids.
   *
   * @tparam T The coordinates type.
   * @return void
   */
  template <class T>
  void compute_tile_offsets();

  /**
   * Computes the tile domain. Applicable only to arrays with regular tiles.
   *
   * @tparam T The domain type.
   * @return void
   */
  template <class T>
  void compute_tile_domain();

  /**
   * Returns the position of the input coordinates inside its corresponding
   * tile, based on the array cell order. Applicable only to **dense** arrays,
   * and focusing on the **column-major** cell order.
   *
   * @tparam T The coordinates type.
   * @param coords The input coordindates, which are expressed as global
   *     coordinates in the array domain.
   * @return The position of the cell coordinates in the array cell order
   *     within its corresponding tile. In case of error, the function returns
   *     TILEDB_AS_ERR.
   */
  template <class T>
  uint64_t get_cell_pos_col(const T* coords) const;

  /**
   * Returns the position of the input coordinates inside its corresponding
   * tile, based on the array cell order. Applicable only to **dense** arrays,
   * and focusing on the **row-major** cell order.
   *
   * @tparam T The coordinates type.
   * @param coords The input coordindates, which are expressed as global
   *     coordinates in the array domain.
   * @return The position of the cell coordinates in the array cell order
   *     within its corresponding tile. In case of error, the function returns
   *     TILEDB_AS_ERR.
   */
  template <class T>
  uint64_t get_cell_pos_row(const T* coords) const;

  /**
   * Retrieves the next coordinates along the array cell order within a given
   * domain (desregarding whether the domain is split into tiles or not).
   * Applicable only to **dense** arrays, and focusing on **column-major**
   * cell order.
   *
   * @tparam T The coordinates type.
   * @param domain The targeted domain.
   * @param cell_coords The input cell coordinates, which the function modifies
   *     to store the next coordinates at termination.
   * @param coords_retrieved Will store true if the retrieved coordinates are
   *     inside the domain, and false otherwise.
   * @return void
   */
  template <class T>
  void get_next_cell_coords_col(
      const T* domain, T* cell_coords, bool& coords_retrieved) const;

  /**
   * Retrieves the next coordinates along the array cell order within a given
   * domain (desregarding whether the domain is split into tiles or not).
   * Applicable only to **dense** arrays, and focusing on **row-major**
   * cell order.
   *
   * @tparam T The coordinates type.
   * @param domain The targeted domain.
   * @param cell_coords The input cell coordinates, which the function modifies
   *     to store the next coordinates at termination.
   * @param coords_retrieved Will store true if the retrieved coordinates are
   *     inside the domain, and false otherwise.
   * @return void
   */
  template <class T>
  void get_next_cell_coords_row(
      const T* domain, T* cell_coords, bool& coords_retrieved) const;

  /**
   * Retrieves the next tile coordinates along the array tile order within a
   * given tile domain. Applicable only to **dense** arrays, and focusing on
   * the **column-major** tile order.
   *
   * @tparam T The coordinates type.
   * @param domain The targeted domain.
   * @param tile_coords The input tile coordinates, which the function modifies
   *     to store the next tile coordinates at termination.
   * @return void
   */
  template <class T>
  void get_next_tile_coords_col(const T* domain, T* tile_coords) const;

  /**
   * Retrieves the next tile coordinates along the array tile order within a
   * given tile domain. Applicable only to **dense** arrays, and focusing on
   * the **row-major** tile order.
   *
   * @tparam T The coordinates type.
   * @param domain The targeted domain.
   * @param tile_coords The input tile coordinates, which the function modifies
   *     to store the next tile coordinates at termination.
   * @return void
   */
  template <class T>
  void get_next_tile_coords_row(const T* domain, T* tile_coords) const;

  /**
   * Retrieves the previous coordinates along the array cell order within a
   * given domain (desregarding whether the domain is split into tiles or not).
   * Applicable only to **dense** arrays, and focusing on the **column-major**
   * cell order.
   *
   * @tparam T The coordinates type.
   * @param domain The targeted domain.
   * @param cell_coords The input cell coordinates, which the function modifies
   *     to store the previous coordinates at termination.
   * @return void
   */
  template <class T>
  void get_previous_cell_coords_col(const T* domain, T* cell_coords) const;

  /**
   * Retrieves the previous coordinates along the array cell order within a
   * given domain (desregarding whether the domain is split into tiles or not).
   * Applicable only to **dense** arrays, and focusing on the **row-major**
   * cell order.
   *
   * @tparam T The coordinates type.
   * @param domain The targeted domain.
   * @param cell_coords The input cell coordinates, which the function modifies
   *     to store the previous coordinates at termination.
   * @return void
   */
  template <class T>
  void get_previous_cell_coords_row(const T* domain, T* cell_coords) const;

  /**
   * Returns the tile position along the array tile order within the input
   * domain. Applicable only to **dense** arrays, and focusing on the
   * **column-major** tile order.
   *
   * @tparam T The domain type.
   * @param tile_coords The tile coordinates.
   * @return The tile position of *tile_coords* along the tile order of the
   *     array inside the array domain.
   */
  template <class T>
  uint64_t get_tile_pos_col(const T* tile_coords) const;

  /**
   * Returns the tile position along the array tile order within the input
   * domain. Applicable only to **dense** arrays, and focusing on the
   * **column-major** tile order.
   *
   * @tparam T The domain type.
   * @param domain The input domain, which is a cell domain partitioned into
   *     regular tiles in the same manner as that of the array domain
   *     (however *domain* may be a sub-domain of the array domain).
   * @param tile_coords The tile coordinates.
   * @return The tile position of *tile_coords* along the tile order of the
   *     array inside the input domain.
   */
  template <class T>
  uint64_t get_tile_pos_col(const T* domain, const T* tile_coords) const;

  /**
   * Returns the tile position along the array tile order within the input
   * domain. Applicable only to **dense** arrays, and focusing on the
   * **row-major** tile order.
   *
   * @tparam T The domain type.
   * @param tile_coords The tile coordinates.
   * @return The tile position of *tile_coords* along the tile order of the
   *     array inside the array domain.
   */
  template <class T>
  uint64_t get_tile_pos_row(const T* tile_coords) const;

  /**
   * Returns the tile position along the array tile order within the input
   * domain. Applicable only to **dense** arrays, and focusing on the
   * **row-major** tile order.
   *
   * @tparam T The domain type.
   * @param domain The input domain, which is a cell domain partitioned into
   *     regular tiles in the same manner as that of the array domain (however
   *     *domain* may be a sub-domain of the array domain).
   * @param tile_coords The tile coordinates.
   * @return The tile position of *tile_coords* along the tile order of the
   *     array inside the input domain.
   */
  template <class T>
  uint64_t get_tile_pos_row(const T* domain, const T* tile_coords) const;

  /** Return the number of cells in a column tile slab of an input subarray. */
  template <class T>
  uint64_t tile_slab_col_cell_num(const T* subarray) const;

  /** Return the number of cells in a row tile slab of an input subarray. */
  template <class T>
  uint64_t tile_slab_row_cell_num(const T* subarray) const;
};

}  // namespace tiledb

#endif  // TILEDB_ARRAY_METADATA_H