/**
 * @file   storage_manager.h
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
 * This file defines class StorageManager.
 */

#ifndef TILEDB_STORAGE_MANAGER_H
#define TILEDB_STORAGE_MANAGER_H

#include <condition_variable>
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "array_metadata.h"
#include "consolidator.h"
#include "locked_array.h"
#include "object_type.h"
#include "open_array.h"
#include "query.h"
#include "status.h"
#include "uri.h"
#include "vfs.h"

namespace tiledb {

/** The storage manager that manages pretty much everything in TileDB. */
class StorageManager {
 public:
  /* ********************************* */
  /*     CONSTRUCTORS & DESTRUCTORS    */
  /* ********************************* */

  /** Constructor. */
  StorageManager();

  /** Destructor. */
  ~StorageManager();

  /* ********************************* */
  /*                API                */
  /* ********************************* */

  /**
   * Consolidates the fragments of an array into a single one.
   *
   * @param array_name The name of the array to be consolidated.
   * @return Status
   */
  Status array_consolidate(const char* array_name);

  /**
   * Creates a TileDB array storing its metadata.
   *
   * @param array_metadata The array metadata.
   * @return Status
   */
  Status array_create(ArrayMetadata* array_metadata);

  /**
   * Locks the array.
   *
   * @param array_uri The array to be locked
   * @param shared True if this is a shared lock, false if it is exclusive.
   * @return Status
   */
  Status array_lock(const URI& array_uri, bool shared);

  /**
   * Unlocks the array.
   *
   * @param array_uri The array to be unlocked
   * @param shared True if this was a shared lock, false if it was exclusive.
   * @return Status
   */
  Status array_unlock(const URI& array_uri, bool shared);

  /**
   * Pushes an async query to the queue.
   *
   * @param query The async query.
   * @param i The index of the thread that executes the function. If it is
   *    equal to 0, it means a user query, whereas if it is 1 it means an
   *    internal query.
   * @return Status
   */
  Status async_push_query(Query* query, int i);

  /** Creates a directory with the input URI. */
  Status create_dir(const URI& uri);

  /** Creates a file with the input URI. */
  Status create_file(const URI& uri);

  /** Deletes a fragment directory. */
  Status delete_fragment(const URI& uri) const;

  /** Safely removes a TileDB resource. */
  Status remove_path(const URI& uri) const;

  /** Safely moves a TileDB resource. */
  Status move(const URI& old_uri, const URI& new_uri, bool force = false) const;

  /** Retrieves the size of the input URI file. */
  Status file_size(const URI& uri, uint64_t* size) const;

  /**
   * Creates a TileDB group.
   *
   * @param group The URI of the group to be created.
   * @return Status
   */
  Status group_create(const std::string& group) const;

  /**
   * Initializes the storage manager. It spawns two threads. The first is for
   * handling user asynchronous queries (submitted via the *query_submit_async*
   * function. The second handles internal asynchronous queries as part of some
   * either sync or async query.
   *
   * @return Status
   */
  Status init();

  /** Returns true if the input URI is a fragment directory. */
  bool is_fragment(const URI& uri) const;

  /** Checks if the input URI is a directory. */
  bool is_dir(const URI& uri);

  /** Checks if the input URI is a file. */
  bool is_file(const URI& uri);

  /**
   * Loads the metadata of an array from persistent storage into memory.
   *
   * @param array_name The name (URI path) of the array.
   * @param array_metadata The array metadata to be retrieved.
   * @return Status
   */
  Status load(const std::string& array_name, ArrayMetadata* array_metadata);

  /**
   * Loads the fragment metadata of an array from persistent storage into
   * memory.
   *
   * @param metadata The fragment metadata to be loaded.
   * @return Status
   */
  Status load(FragmentMetadata* metadata);

  /**
   * TODO: DOC
   * @param old_uri
   * @param new_uri
   * @param force
   * @return
   */
  Status move_path(const URI& old_uri, const URI& new_uri, bool force = false);

  /** Finalizes a query. */
  Status query_finalize(Query* query);

  /**
   * Initializes a query.
   *
   * @param query The query to initialize.
   * @param array_name The name of the array the query targets at.
   * @param type The query type.
   * @param layout The cell layout.
   * @param subarray The subarray the query will be constrained on.
   * @param attributes The attributes the query will be constrained on.
   * @param attribute_num The number of attributes.
   * @param buffers The buffers that will hold the cells to write, or will
   *     hold the cells that will be read.
   * @param buffer_sizes The corresponding buffer sizes.
   * @param consolidation_fragment_uri This is used only in write queries.
   *     If it is different than empty, then it indicates that the query will
   *     be writing into a consolidation fragment with the input name.
   * @return Status
   */
  Status query_init(
      Query* query,
      const char* array_name,
      QueryType type,
      Layout layout,
      const void* subarray,
      const char** attributes,
      unsigned int attribute_num,
      void** buffers,
      uint64_t* buffer_sizes,
      const URI& consolidation_fragment_uri = URI(""));

  /** Submits a query for (sync) execution. */
  Status query_submit(Query* query);

  /**
   * Submits a query for async execution.
   *
   * @param query The query to submit.
   * @param callback The fuction that will be called upon query completion.
   * @param callback_data The data to be provided to the callback function.
   * @return Status
   */
  Status query_submit_async(
      Query* query, void* (*callback)(void*), void* callback_data);

  /**
   * Reads from a file into the input buffer.
   *
   * @param uri The URI file to read from.
   * @param offset The offset in the file the read will start from.
   * @param buffer The buffer to write into. The function reallocates memory
   *     for the buffer, sets its size to *nbytes* and resets its offset.
   * @param nbytes The number of bytes to read.
   * @return Status.
   */
  Status read_from_file(
      const URI& uri, uint64_t offset, Buffer* buffer, uint64_t nbytes) const;

  /**
   * Stores an array metadata into persistent storage.
   *
   * @param array_metadata The array metadata to be stored.
   * @return Status
   */
  Status store(ArrayMetadata* array_metadata);

  /**
   * Stores the fragment metadata into persistent storage.
   *
   * @param metadata The fragment metadata to be stored.
   * @return Status
   */
  Status store(FragmentMetadata* metadata);

  /**
   * Syncs a URI (file or directory), i.e., commits its contents
   * to persistent storage.
   */
  Status sync(const URI& uri);

  /**
   * Writes the contents of a buffer into a URI file.
   *
   * @param uri The file to write into.
   * @param buffer The buffer to write.
   * @return Status.
   */
  Status write_to_file(const URI& uri, Buffer* buffer) const;

  /**
   * Returns the tiledb object type
   * @param uri Path to tiledb object resource
   * @return ObjectType
   */
  ObjectType object_type(const URI& uri) const;

 private:
  /* ********************************* */
  /*        PRIVATE ATTRIBUTES         */
  /* ********************************* */

  /**
   * Async condition variable. The first is for user async queries, the second
   * for internal async queries.
   * */
  std::condition_variable async_cv_[2];

  /** If true, the async thread will be eventually terminated. */
  bool async_done_;

  /**
   * Async query queue. The first is for user queries, the second for
   * internal queries. The queries are processed in a FIFO manner.
   */
  std::queue<Query*> async_queue_[2];

  /**
   * Async mutex. The first is for the user queries thread, the second for
   * the internal queries thread.
   */
  std::mutex async_mtx_[2];

  /**
   * Threads that handle all async queries. The first is for user queries,
   * the second for internal queries.
   */
  std::thread* async_thread_[2];

  /** Object that handles array consolidation. */
  Consolidator* consolidator_;

  /** Used for array shared and exclusive locking. */
  std::mutex locked_array_mtx_;

  /** Used for array shared and exclusive locking. */
  std::condition_variable locked_array_cv_;

  /**
   * Stores locked array entries. The map is indexed by the array URI string
   * and stores the number of **shared** locks.
   */
  std::map<std::string, LockedArray*> locked_arrays_;

  /** Mutex for managing OpenArray objects. */
  std::mutex open_array_mtx_;

  /**
   * Stores the currently open arrays. An array is *opened* when a new query is
   * initialized via *query_init* for a particular array.
   */
  std::map<std::string, OpenArray*> open_arrays_;

  /**
   * Virtual filesystem handler. It directs queries to the appropriate
   * filesystem backend. Note that this is stateful.
   */
  VFS* vfs_;

  /* ********************************* */
  /*         PRIVATE METHODS           */
  /* ********************************* */

  /** Closes an array, properly managing the opened fragment metadata. */
  Status array_close(
      URI array, const std::vector<FragmentMetadata*>& fragment_metadata);

  /**
   * Opens an array, retrieving its metadata and fragment metadata.
   *
   * @param array_uri The array URI.
   * @param query_type The query type.
   * @param subarray The subarray the query is constrained on.
   * @param array_metadata The array metadata to be retrieved.
   * @param fragment_metadata The fragment metadat to be retrieved.
   * @return
   */
  Status array_open(
      const URI& array_uri,
      QueryType query_type,
      const void* subarray,
      const ArrayMetadata** array_metadata,
      std::vector<FragmentMetadata*>* fragment_metadata);

  /**
   * Invokes in case an error occurs in array_open. It is a clean-up function.
   */
  Status array_open_error(
      OpenArray* open_array,
      const std::vector<FragmentMetadata*>& fragment_metadata);

  /**
   * Starts listening to async queries.
   *
   * @param storage_manager The storage manager object that handles the
   *     async query threads.
   * @param i The index of the thread to execute the function. If it is
   *     equal to 0, it means a user query, whereas if it is 1 it means an
   *     internal query.
   */
  static void async_start(StorageManager* storage_manager, int i = 0);

  /** Stops listening to async queries. */
  void async_stop();

  /** Handles a single async query. */
  void async_process_query(Query* query);

  /**
   * Starts handling async queries.
   *
   * @param i The index of the thread that executes the function. If it is
   * equal to 0, it means a user query, whereas if it is 1 it means an
   * internal query.
   */
  void async_process_queries(int i);

  /**
   * Retrieves the fragment URI's of an array that overlap with a give
   * subarray.
   */
  // TODO: Currently, no overlap check is performed with the subarray
  // TODO: and all fragments are retrieved. To be fixed soon.
  Status get_fragment_uris(
      const URI& array_uri,
      const void* subarray,
      std::vector<URI>* fragment_uris) const;

  /** Retrieves an open array entry for the given array URI. */
  Status open_array_get_entry(const URI& array_uri, OpenArray** open_array);

  /** Loads the array metadata into an open array. */
  Status open_array_load_metadata(const URI& array_uri, OpenArray* open_array);

  /** Retrieves the fragment metadata of an open array for a given subarray. */
  Status open_array_load_fragment_metadata(
      OpenArray* open_array,
      const void* subarray,
      std::vector<FragmentMetadata*>* fragment_metadata);

  /**
   * Sorts the input fragment URIs in ascending timestamp order, breaking
   * ties using the process id.
   */
  void sort_fragment_uris(std::vector<URI>* fragment_uris) const;
};

}  // namespace tiledb

#endif  // TILEDB_STORAGE_MANAGER_H
