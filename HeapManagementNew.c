#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define HeapSize 900
char Heap[HeapSize];                            //Statically allocated heap array(heap storage)

typedef struct MetaData
{
    size_t size;           // Size of usable memory in this block (excluding metadata)
    char status;           // 'f' for free, 'a' for allocated
    struct MetaData *next; // Next block in the list
} MetaBlock;

MetaBlock *heap_block_ptr = (void *)Heap;        //statically creating the block(head to heap array)

void Initialize()
{
    heap_block_ptr->size = (HeapSize - sizeof(MetaBlock));
    heap_block_ptr->status = 'f';
    heap_block_ptr->next = NULL;

    printf("Heap Allocator Initialized\n");
    printf("  - Total heap size: %d bytes\n", HeapSize);
    printf("  - Metadata size: %zu bytes per block\n", sizeof(MetaBlock));
    printf("  - Available memory: %zu bytes\n", heap_block_ptr->size);
    printf("  - Heap start address: %p\n", heap_block_ptr);
    printf("  - First usable memory: %p\n", (void *)heap_block_ptr + sizeof(MetaBlock));
}

void *Allocate(size_t size_to_be_allocated)
{
    if (size_to_be_allocated == 0)
    {
        printf("Cannot allocate 0 bytes\n");
        return NULL;
    }

    MetaBlock *current = heap_block_ptr;
    void *ret_ptr = NULL;

    while (current != NULL) // Search for a suitable block
    {
        if (current->status == 'f' && current->size >= size_to_be_allocated)            
        {
            break; // Found a suitable block
        }
        current = current->next;
    }

    if (current == NULL)
    {
        printf("No free space left to allocate %zu bytes of memory\n", size_to_be_allocated);
        return NULL;
    }

    if (current->size > size_to_be_allocated + sizeof(MetaBlock)) // Check if we need to split this block(TO avoid internal fragmentation)
    {
        // Calculate address for the new metadata block
        //statically creating the new block
        MetaBlock *new_block = (void *)((void *)current + sizeof(MetaBlock) + size_to_be_allocated);

        new_block->size = current->size - size_to_be_allocated - sizeof(MetaBlock); // Set up the new block
        new_block->next = current->next;
        new_block->status = 'f';

        current->size = size_to_be_allocated; // Update the current block
        current->next = new_block;

        printf("Split block: allocated %zu bytes, created new free block of %zu bytes\n",
               size_to_be_allocated, new_block->size);
    }

    current->status = 'a'; // Mark this block as allocated, internal fragmentation may be their if their is no splitting

    ret_ptr = (void *)current + sizeof(MetaBlock); // Return pointer to the usable memory after the metadata
    printf("Successfully allocated %zu bytes at address %p\n", size_to_be_allocated, ret_ptr);

    return ret_ptr;
}

MetaBlock *GetBlockMetadata(void *ptr)
{
    if (ptr == NULL)
    {
        return NULL;
    }

    if (ptr < (void *)Heap || ptr >= (void *)(Heap + HeapSize)) // Check if pointer is within heap bounds
    {
        return NULL;
    }

    MetaBlock *current = heap_block_ptr; // Find the metadata block for this pointer

    while (current != NULL)
    {
        int block_data = (void *)current + sizeof(MetaBlock);
        if (block_data == ptr) // If this is the block we want
        {
            return current;
        }
        current = current->next;
    }
    return NULL; // Not found
}

bool Free(void *ptr_to_free)
{
    if (ptr_to_free == NULL)
    {
        printf("Cannot free NULL pointer\n");
        return false;
    }
    MetaBlock *block_to_free = GetBlockMetadata(ptr_to_free);
    if (block_to_free == NULL)
    {
        printf("Invalid pointer: not the start of an allocated block\n");
        return false;
    }
    if (block_to_free->status != 'a')
    {
        printf("Double free detected or block already free\n");
        return false;
    }
    block_to_free->status = 'f'; // Mark as free
    printf("Successfully freed block of size %zu bytes at address %p\n",
           block_to_free->size, ptr_to_free);
    return true;
}

void Merge()                        //To reduce external fragmentation
{
    MetaBlock *current = heap_block_ptr;
    int merges_done = 0;

    while (current != NULL && current->next != NULL)
    {
        if (current->status == 'f' && current->next->status == 'f')
        {
            // Calculate the total size including the metadata we'll reclaim
            size_t new_size = current->size + current->next->size + sizeof(MetaBlock);
            MetaBlock *to_be_merged = current->next;

            // Update current block to absorb the next one
            current->size = new_size;
            current->next = to_be_merged->next;

            merges_done++;
            // Don't advance current pointer as we might need to merge again
        }
        else
        {
            // Move to next block
            current = current->next;
        }
    }
    if (merges_done > 0)
    {
        printf("Merged %d adjacent free blocks\n", merges_done);
    }
}

void DisplayHeap()
{
    MetaBlock *current = heap_block_ptr;
    int block_count = 0;
    size_t total_free = 0;
    size_t total_allocated = 0;

    printf("\n===== HEAP MEMORY MAP =====\n");
    printf("%-5s %-20s %-10s %-10s %-15s\n",
           "Block", "Address", "Status", "Size", "Usable Memory");
    printf("----------------------------------------------------------------\n");

    while (current != NULL)
    {
        printf("%-5d %-20p %-10c %-10zu %-15p\n",
               block_count,
               (void *)current,
               current->status,
               current->size,
               (void *)current + sizeof(MetaBlock));

        if (current->status == 'f')
        {
            total_free += current->size;
        }
        else
        {
            total_allocated += current->size;
        }

        block_count++;
        current = current->next;
    }
    printf("----------------------------------------------------------------\n");
    printf("Total blocks: %d\n", block_count);
    printf("Total allocated: %zu bytes\n", total_allocated);
    printf("Total free: %zu bytes\n", total_free);
    printf("Metadata overhead: %zu bytes\n", block_count * sizeof(MetaBlock));
    printf("============================\n\n");
}

void DisplayMenu()
{
    printf("\n===== HEAP ALLOCATOR MENU =====\n");
    printf("1. Allocate memory\n");
    printf("2. Free memory\n");
    printf("3. Display heap status\n");
    printf("4. Exit\n");
    printf("Enter your choice (1-4): ");
}

int main()
{
    Initialize(); // Initialize the heap

    void *allocated_ptrs[100];                   // Keep track of allocated pointers for easier freeing
    size_t allocated_sizes[100];
    int ptr_count = 0;

    int choice;
    size_t size;
    int index;

    do
    {
        DisplayMenu();
        scanf("%d", &choice);

        switch (choice)
        {
        case 1: // Allocate
            printf("Enter size to allocate (in bytes): ");
            scanf("%zu", &size);

            void *ptr = Allocate(size);
            if (ptr != NULL && ptr_count < 100)
            {
                allocated_ptrs[ptr_count] = ptr;
                allocated_sizes[ptr_count] = size;
                printf("Allocated pointer #%d\n", ptr_count);
                ptr_count++;
            }
            break;

        case 2: // Free
            if (ptr_count == 0)
            {
                printf("No allocated pointers to free\n");
                break;
            }

            printf("Currently allocated pointers:\n");
            for (int i = 0; i < ptr_count; i++)
            {
                printf("%d: Address %p, Size %zu bytes\n",
                       i, allocated_ptrs[i], allocated_sizes[i]);
            }

            printf("Enter index of pointer to free (0-%d): ", ptr_count - 1);
            scanf("%d", &index);

            if (index >= 0 && index < ptr_count)
            {
                if (Free(allocated_ptrs[index]))
                {
                    // Remove this pointer from our tracking array
                    for (int i = index; i < ptr_count - 1; i++)
                    {
                        allocated_ptrs[i] = allocated_ptrs[i + 1];
                        allocated_sizes[i] = allocated_sizes[i + 1];
                    }
                    ptr_count--;

                    // Merge adjacent free blocks
                    Merge();
                }
            }
            else
            {
                printf("Invalid index\n");
            }
            break;

        case 3: // Display heap
            DisplayHeap();
            break;

        case 4: // Exit
            printf("Exiting...\n");
            break;

        default:
            printf("Invalid choice. Please enter 1-4.\n");
        }

    } while (choice != 4);

    return 0;
}