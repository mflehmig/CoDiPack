/*
 * CoDiPack, a Code Differentiation Package
 *
 * Copyright (C) 2015-2018 Chair for Scientific Computing (SciComp), TU Kaiserslautern
 * Homepage: http://www.scicomp.uni-kl.de
 * Contact:  Prof. Nicolas R. Gauger (codi@scicomp.uni-kl.de)
 *
 * Lead developers: Max Sagebaum, Tim Albring (SciComp, TU Kaiserslautern)
 *
 * This file is part of CoDiPack (http://www.scicomp.uni-kl.de/software/codi).
 *
 * CoDiPack is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * CoDiPack is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU
 * General Public License along with CoDiPack.
 * If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Max Sagebaum, Tim Albring, (SciComp, TU Kaiserslautern)
 */

#pragma once

#include <vector>

#include "../../tools/tapeValues.hpp"

/**
 * @brief Global namespace for CoDiPack - Code Differentiation Package
 */
namespace codi {

  /**
   * @brief Handles the indices that can be used and reused.
   *
   * New indices are generated if required. All the freed indices are stored in a list
   * and are reused when indices are needed.
   *
   * @tparam IndexType  The type for the handled indices.
   */
  template<typename IndexType>
  class ReuseIndexHandler {
    public:
      /**
       * @brief The type definition for other tapes who want to access the type.
       */
      typedef IndexType Index;


      /**
       * @brief If it is required to write an assign statement after the index is copied.
       */
      const static bool AssignNeedsStatement = true;

    private:

      /** @brief The maximum index that was used over the whole process */
      Index globalMaximumIndex;
      /**
       * @brief The maximum index that is currently used.
       *
       * The maximum is decremented every time an index is released that corresponds to the
       * current maximum.
       */
      Index currentMaximumIndex;

      /**
       * @brief The list with the indices that are available for reuse.
       */
      std::vector<Index> freeIndices;

    public:

      /**
       * @brief Create a handler for index reuse.
       *
       * The argument reserveIndices will cause the index manager to reserve the first n indices, so that there are
       * not used by the index manager and are freely available to anybody.
       *
       * @param[in] reserveIndices  The number of indices that are reserved and not used by the manager.
       */
      ReuseIndexHandler(const Index reserveIndices) :
        globalMaximumIndex(reserveIndices),
        currentMaximumIndex(reserveIndices),
        freeIndices() {}

      /**
       * @brief Free the index that is given to the method.
       *
       * The method checks if the index is equal to the current maximum. If yes
       * then the current maximum is decremented otherwise the index is added
       * to the freed list.
       *
       * @param[in,out] index  The index that is freed. It is set to zero in the method.
       */
      CODI_INLINE void freeIndex(Index& index) {
        if(0 != index) { // do not free the zero index

#if CODI_IndexHandle
        handleIndexFree(index);
#endif
          if(currentMaximumIndex == index) {
            // freed index is the maximum one so we can decrease the count
            --currentMaximumIndex;
          } else {
            freeIndices.push_back(index);
          }

          index = 0;
        }
      }

      /**
       * @brief Generate a new index.
       *
       * @return The new index that can be used.
       */
      CODI_INLINE Index createIndex() {
        Index index;
        if(0 != freeIndices.size()) {
          index = freeIndices.back();
          freeIndices.pop_back();
        } else {
          if(globalMaximumIndex == currentMaximumIndex) {
            ++globalMaximumIndex;
          }
          index = ++currentMaximumIndex;
        }

#if CODI_IndexHandle
        handleIndexCreate(index);
#endif

        return index;
      }

      /**
       * @brief Check if the index is active if not a new index is generated.
       *
       * @param[in,out] index The current value of the index. If 0 then a new index is generated.
       */
      CODI_INLINE void assignIndex(Index& index) {
        if(0 == index) {
          index = this->createIndex();
        }
      }

      /**
       * @brief The index on the rhs is ignored in this manager.
       *
       * The manager ensures only that the lhs index is valid.
       */
      CODI_INLINE void copyIndex(Index& lhs, const Index& rhs) {
        CODI_UNUSED(rhs);
        assignIndex(lhs);
      }

      /**
       * @brief Not needed by this index manager.
       */
      CODI_INLINE void reset() const {
        /* do nothing */
      }

      /**
       * @brief Get the maximum global
       *
       * @return The maximum index that was used during the lifetime of this index handler.
       */
      CODI_INLINE Index getMaximumGlobalIndex() const {
        return globalMaximumIndex;
      }

      /**
       * @brief Get the current maximum index.
       *
       * @return The current maximum index that is in use.
       */
      CODI_INLINE Index getCurrentIndex() const {
        return currentMaximumIndex;
      }

      /**
       * @brief Get the number of the stored indices.
       *
       * @return The number of stored indices.
       */
      size_t getNumberStoredIndices() const {
        return freeIndices.size();
      }

      /**
       * @brief Get the number of the allocated indices.
       *
       * @return The number of the allocated indices.
       */
      size_t getNumberAllocatedIndices() const {
        return freeIndices.capacity();
      }

      /**
       * @brief Add statistics about the used indices.
       *
       * Adds the
       *   maximum number of live indices,
       *   the current number of lives indices,
       *   the indices that are stored and
       *   the memory for the allocated indices.
       *
       * @param[in,out] values  The values where the information is added to.
       */
      void addValues(TapeValues& values) const {
        size_t maximumGlobalIndex     = (size_t)this->getMaximumGlobalIndex();
        size_t storedIndices          = (size_t)this->getNumberStoredIndices();
        size_t currentLiveIndices     = (size_t)this->getCurrentIndex() - this->getNumberStoredIndices();

        double memoryStoredIndices    = (double)storedIndices*(double)(sizeof(Index)) * BYTE_TO_MB;
        double memoryAllocatedIndices = (double)this->getNumberAllocatedIndices()*(double)(sizeof(Index)) * BYTE_TO_MB;

        values.addSection("Indices");
        values.addData("Max. live indices", maximumGlobalIndex);
        values.addData("Cur. live indices", currentLiveIndices);
        values.addData("Indices stored", storedIndices);
        values.addData("Memory used", memoryStoredIndices, true, false);
        values.addData("Memory allocated", memoryAllocatedIndices, false, true);
      }
  };
}
