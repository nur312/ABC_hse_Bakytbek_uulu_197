;Bakytbek uulu Nurzhigit
;197
;variant 3

format PE console
entry start

include 'win32a.inc'

;--------------------------------------------------------------------------
section '.data' data readable writable

        strVecSize   db 'size of vector? ',10, 0
        strIncorSize db 'Incorrect size of vector', 10, 0
        strVecElemI  db '[%d]? ', 0
        strScanInt   db '%d', 0
       strVecElemOut db '[%d] = %d', 10, 0
        strNewVec    db 'New vector:', 10, 0
        vec_size     dd 0
        sum          dd 0
        i            dd ?
        tmp          dd ?
        tmpStack     dd ?
        vec          rd 101
        res_vec      rd 100

;--------------------------------------------------------------------------
section '.code' code readable executable
start:
; 1) vector input
        call VectorInput
; 2) create new array where b[i] = a[i] + a[i+1] (b -new array, a - input array)
        call ResultVector
; 3) print result vector
        call ResVectorOut
finish:
        call [getch]

        push 0
        call [ExitProcess]

;--------------------------------------------------------------------------
VectorInput:
        push strVecSize
        call [printf]
        add esp, 4

        push vec_size
        push strScanInt
        call [scanf]
        add esp, 8

        cmp eax, 0
        jg  getVector

; fail size

        push strIncorSize
        call [printf]
        call [getch]
        push 0
        call [ExitProcess]
; else continue...
getVector:
        xor ecx, ecx            ; ecx = 0
        mov ebx, vec            ; ebx = &vec
getVecLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        jge endInputVector       ; to end of loop

        ; input element
        mov [i], ecx
        push ecx
        push strVecElemI
        call [printf]
        add esp, 8

        push ebx
        push strScanInt
        call [scanf]
        add esp, 8

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp getVecLoop
endInputVector:
        ret
;--------------------------------------------------------------------------
ResultVector:
        xor ecx, ecx            ; ecx = 0
        mov ebx, vec            ; ebx = &res_vec
        mov edx, res_vec
resVecLoop:
        cmp ecx, [vec_size]
        je endResVector      ; to end of loop
        ; b[i] = a[i] + a[i+1]
        mov eax, [ebx]
        add ebx, 4
        add eax, [ebx]
        mov [edx], eax

        inc ecx
        add edx, 4
        jmp resVecLoop
endResVector:
        ret

;--------------------------------------------------------------------------
ResVectorOut:
        push strNewVec
        call [printf]
        add esp, 4
        mov [tmpStack], esp
        xor ecx, ecx            ; ecx = 0
        mov ebx, res_vec        ; ebx = &res_vec
putVecLoop:
        mov [tmp], ebx
        cmp ecx, [vec_size]
        je endOutputVector      ; to end of loop
        mov [i], ecx

        ; output element
        push dword [ebx]
        push ecx
        push strVecElemOut
        call [printf]

        mov ecx, [i]
        inc ecx
        mov ebx, [tmp]
        add ebx, 4
        jmp putVecLoop
endOutputVector:
        mov esp, [tmpStack]
        ret
;-------------------------------third act - including HeapApi--------------------------
                                                 
section '.idata' import data readable
    library kernel, 'kernel32.dll',\
            msvcrt, 'msvcrt.dll',\
            user32,'USER32.DLL'

include 'api\user32.inc'
include 'api\kernel32.inc'
    import kernel,\
           ExitProcess, 'ExitProcess',\
           HeapCreate,'HeapCreate',\
           HeapAlloc,'HeapAlloc'
  include 'api\kernel32.inc'
    import msvcrt,\
           printf, 'printf',\
           scanf, 'scanf',\
           getch, '_getch'