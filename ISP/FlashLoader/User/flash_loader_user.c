//------------------------------------------------------------------------------
//
// Copyright (c) 2008-2015 IAR Systems
//
// Licensed under the Apache License, Version 2.0 (the "License")
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// $Revision: 38952 $
//
//------------------------------------------------------------------------------

#include "flash_loader.h"
#include "flash_loader_extra.h"

#include "sys.h"
#include "qspi.h"
#include "w25qxx.h"

__no_init uint32_t base_addr;

uint32_t FlashInit(void *base_of_flash, uint32_t image_size,
                   uint32_t link_address, uint32_t flags,
                   int argc, char const *argv[])
{
    Stm32_Clock_Init(100,1,2,2);
    W25QXX_Init();
    base_addr = (uint32_t)base_of_flash;
    return RESULT_OK;
}

uint32_t FlashWrite(void *block_start,
                    uint32_t offset_into_block,
                    uint32_t count,
                    char const *buffer)
{
    u32 dest = (u32)block_start + offset_into_block - base_addr;
    W25QXX_Write((u8 *)buffer, dest, count);
    return 0;
}

uint32_t FlashErase(void *block_start,
                    uint32_t block_size)
{
  W25QXX_Erase_Sector((u32)block_start - base_addr);
  return 0;
}

OPTIONAL_SIGNOFF
uint32_t FlashSignoff(void)
{
  if(W25QXX_QPI_MODE) W25QXX_Qspi_Disable();
  return 0;
}
