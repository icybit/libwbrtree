/*
 * hashset_itr.c
 *
 *   Copyright 2012 Couchbase, Inc.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#include <assert.h>
#include <stdio.h>
#include "hashset_itr.h"

hashset_itr_t hashset_iterator(hashset_t set)
{
  hashset_itr_t itr = calloc(1, sizeof(struct hashset_itr_st));
  if (!itr)
  {
    return NULL;
  }

  itr->set = set;
  itr->index = -1;

  return itr;
}

int hashset_iterator_has_next(hashset_itr_t itr)
{
  int index;

  if (itr->set->nitems == 0)
  {
    return 0;
  }

  index = itr->index;
  
  while (index < ((int)itr->set->capacity - 1))
  {
    if(itr->set->items[++index])
    {
      return 1;
    }
  }

  return 0;
}

int hashset_iterator_next(hashset_itr_t itr)
{  
  do
  {
    itr->index++;
  }
  while (!itr->set->items[itr->index] && itr->index < itr->set->capacity - 1);

  return itr->index;
}

size_t hashset_iterator_value(hashset_itr_t itr) 
{  
  if(itr->index == -1 || !itr->set->items[itr->index])
  {
    hashset_iterator_next(itr);
  }

  return itr->set->items[itr->index];
}

