#pragma once
#include "YBaseLib/Common.h"
#include "YBaseLib/CString.h"
#include "YBaseLib/Assert.h"
#include "YBaseLib/Memory.h"

//
// String
// Implements a UTF-8 string container with copy-on-write behavior.
// The data class is not currently threadsafe (creating a mutex on each container would be overkill),
// so locking is still required when multiple threads are involved.
//
class String
{
public:
    // Internal StringData class.
    struct StringData
    {
        // Pointer to memory where the string is located
        char *pBuffer;

        // Length of the string located in pBuffer (in characters)
        uint32 StringLength;

        // Size of the buffer pointed to by pBuffer
        uint32 BufferSize;

        // Whether the memory pointed to by pBuffer is writable.
        bool ReadOnly;

        // Reference count of this data object. If set to -1,
        // it is considered noncopyable and any copies of the string
        // will always create their own copy.
        ALIGN_DECL(4) volatile int32 ReferenceCount;
    };

public:
    // Creates an empty string.
    String();

    // Creates a string containing the specified text.
    // Note that this will incur a heap allocation, even if Text is on the stack.
    // For strings that do not allocate any space on the heap, see StaticString.
    String(const char *Text);

    // Creates a string using the same buffer as another string (copy-on-write).
    String(const String &copyString);

    // Destructor. Child classes may not have any destructors, as this is not virtual.
    ~String();

    // manual assignment
    void Assign(const String &copyString);
    void Assign(const char *copyText);

    // assignment but ensures that we have our own copy.
    void AssignCopy(const String &copyString);

    // Ensures that the string has its own unique copy of the data.
    void EnsureOwnWritableCopy();

    // Ensures that we have our own copy of the buffer, and spaceRequired bytes free in the buffer.
    void EnsureRemainingSpace(uint32 spaceRequired);

    // clears the contents of the string
    void Clear();

    // clear the contents of the string, and free any memory currently being used
    void Obliterate();

    // swaps strings
    void Swap(String &swapString);

    // append a single character to this string
    void AppendCharacter(char c);

    // append a string to this string
    void AppendString(const String &appendStr);
    void AppendString(const char *appendText);
    void AppendString(const char *appendString, uint32 Count);

    // append a substring of the specified string to this string
    void AppendSubString(const String &appendStr, int32 Offset = 0, int32 Count = INT_MAX);
    void AppendSubString(const char *appendText, int32 Offset = 0, int32 Count = INT_MAX);
    
    // append formatted string to this string
    void AppendFormattedString(const char *FormatString, ...);
    void AppendFormattedStringVA(const char *FormatString, va_list ArgPtr);

    // append a single character to this string
    void PrependCharacter(char c);

    // append a string to this string
    void PrependString(const String &appendStr);
    void PrependString(const char *appendText);
    void PrependString(const char *appendString, uint32 Count);

    // append a substring of the specified string to this string
    void PrependSubString(const String &appendStr, int32 Offset = 0, int32 Count = INT_MAX);
    void PrependSubString(const char *appendText, int32 Offset = 0, int32 Count = INT_MAX);

    // append formatted string to this string
    void PrependFormattedString(const char *FormatString, ...);
    void PrependFormattedStringVA(const char *FormatString, va_list ArgPtr);

    // insert a string at the specified offset
    void InsertString(int32 offset, const String &appendStr);
    void InsertString(int32 offset, const char *appendStr);
    void InsertString(int32 offset, const char *appendStr, uint32 appendStrLength);

    // set to formatted string
    void Format(const char *FormatString, ...);
    void FormatVA(const char *FormatString, va_list ArgPtr);

    // compare one string to another
    bool Compare(const String &otherString) const;
    bool Compare(const char *otherText) const;
    bool SubCompare(const String &otherString, uint32 Length) const;
    bool SubCompare(const char *otherText, uint32 Length) const;
    bool CompareInsensitive(const String &otherString) const;
    bool CompareInsensitive(const char *otherText) const;
    bool SubCompareInsensitive(const String &otherString, uint32 Length) const;
    bool SubCompareInsensitive(const char *otherText, uint32 Length) const;

	// numerical compares
	int NumericCompare(const String &otherString) const;
    int NumericCompare(const char *otherText) const;	
	int NumericCompareInsensitive(const String &otherString) const;
	int NumericCompareInsensitive(const char *otherText) const;

    // starts with / ends with
    bool StartsWith(const char *compareString, bool caseSensitive = true) const;
    bool StartsWith(const String &compareString, bool caseSensitive = true) const;
    bool EndsWith(const char *compareString, bool caseSensitive = true) const;
    bool EndsWith(const String &compareString, bool caseSensitive = true) const;

    // searches for a character inside a string
    // rfind is the same except it starts at the end instead of the start
    // returns -1 if it is not found, otherwise the offset in the string
    int32 Find(char c, uint32 Offset = 0) const;
    int32 RFind(char c, uint32 Offset = 0) const;

    // searches for a string inside a string
    // rfind is the same except it starts at the end instead of the start
    // returns -1 if it is not found, otherwise the offset in the string
    int32 Find(const char *str, uint32 Offset = 0) const;

    // alters the length of the string to be at least len bytes long
    void Reserve(uint32 newReserve, bool Force = false);

    // Cuts characters off the string to reduce it to len bytes long.
    void Resize(uint32 newSize, char fillerCharacter = ' ', bool skrinkIfSmaller = false);

    // updates the internal length counter when the string is externally modified
    void UpdateSize();

    // shrink the string to the minimum size possible
    void Shrink(bool Force = false);

    // gets the size of the string
    uint32 GetLength() const { return m_pStringData->StringLength; }
    bool IsEmpty() const { return (m_pStringData->StringLength == 0); }

    // gets the maximum number of bytes we can write to the string, currently
    uint32 GetBufferSize() const { return m_pStringData->BufferSize; }
    uint32 GetWritableBufferSize() { EnsureOwnWritableCopy(); return m_pStringData->BufferSize; }

    // creates a new string using part of this string
    String SubString(int32 Offset, int32 Count = INT_MAX) const;

    // erase count characters at offset from this string. if count is less than zero, everything past offset is erased
    void Erase(int32 Offset, int32 Count = INT_MAX);

    // replaces all instances of character c with character r in this string
    // returns the number of changes
    uint32 Replace(char searchCharacter, char replaceCharacter);

    // replaces all instances of string s with string r in this string
    // returns the number of changes
    uint32 Replace(const char *searchString, const char *replaceString);

    // convert string to lowercase
    void ToLower();

    // convert string to upper
    void ToUpper();

    // strip characters from start and end of the string
    void LStrip(const char *szStripCharacters = " \t\r\n");
    void RStrip(const char *szStripCharacters = " \t\r\n");
    void Strip(const char *szStripCharacters = " \t\r\n");

    // gets a constant pointer to the string
    const char *GetCharArray() const { return m_pStringData->pBuffer; }

    // gets a writable char array, do not write more than reserve characters to it.
    char *GetWriteableCharArray() { EnsureOwnWritableCopy(); return m_pStringData->pBuffer; }

    // creates a new string from the specified format
    static String FromFormat(const char *FormatString, ...);

    // accessor operators
    //const char &operator[](uint32 i) const { DebugAssert(i < m_pStringData->StringLength); return m_pStringData->pBuffer[i]; }
    //char &operator[](uint32 i) { DebugAssert(i < m_pStringData->StringLength); return m_pStringData->pBuffer[i]; }
    operator const char *() const { return GetCharArray(); }
    operator char *() { return GetWriteableCharArray(); }

    // Will use the string data provided.
    String &operator=(const String &copyString) { Assign(copyString); return *this; }

    // Allocates own buffer and copies text.
    String &operator=(const char *Text) { Assign(Text); return *this; }

    // comparative operators
    bool operator==(const String &compString) const { return Compare(compString); }
    bool operator==(const char *compString) const { return Compare(compString); }
    bool operator!=(const String &compString) const { return !Compare(compString); }
    bool operator!=(const char *compString) const { return !Compare(compString); }
    bool operator<(const String &compString) const { return (NumericCompare(compString) < 0); }
    bool operator<(const char *compString) const { return (NumericCompare(compString) < 0); }
    bool operator>(const String &compString) const { return (NumericCompare(compString) > 0); }
    bool operator>(const char *compString) const { return (NumericCompare(compString) > 0); }

protected:
    // Hidden constructor for creating string child classes.
    // It does not increment the reference count on the string data, therefore dangerous to be public.
    String(StringData *pStringData) : m_pStringData(pStringData) {}

    // Internal append function.
    void InternalPrepend(const char *pString, uint32 Length);
    void InternalAppend(const char *pString, uint32 Length);

    // Pointer to string data.
    StringData *m_pStringData;

    // Empty string data.
    static const StringData s_EmptyStringData;
};

// static string, stored in .rodata
class StaticString : public String
{
public:
    StaticString(const char *Text) : String(&m_sStringData)
    {
        m_sStringData.pBuffer = const_cast<char *>(Text);
        m_sStringData.StringLength = Y_strlen(Text);
        m_sStringData.BufferSize = m_sStringData.StringLength + 1;
        m_sStringData.ReadOnly = true;
        m_sStringData.ReferenceCount = -1;
    }

private:
    StringData m_sStringData;
};

// stack-allocated string
template<uint32 L>
class StackString : public String
{
public:
    StackString() : String(&m_sStringData)
    {
        InitStackStringData();
    }

    StackString(const char *Text) : String(&m_sStringData)
    {
        InitStackStringData();
        Assign(Text);
    }

    StackString(const String &copyString) : String(&m_sStringData)
    {
        // force a copy by passing it a string pointer, instead of a string object
        InitStackStringData();
        Assign(copyString.GetCharArray());
    }

    StackString(const StackString &copyString) : String(&m_sStringData)
    {
        // force a copy by passing it a string pointer, instead of a string object
        InitStackStringData();
        Assign(copyString.GetCharArray());
    }

    // Override the fromstring method
    static StackString FromFormat(const char *FormatString, ...)
    {
        va_list argPtr;
        va_start(argPtr, FormatString);

        StackString returnValue;
        returnValue.FormatVA(FormatString, argPtr);

        va_end(argPtr);

        return returnValue;
    }

    // Will use the string data provided.
    StackString &operator=(const StackString &copyString) { Assign(copyString.GetCharArray()); return *this; }
    StackString &operator=(const String &copyString) { Assign(copyString.GetCharArray()); return *this; }

    // Allocates own buffer and copies text.
    StackString &operator=(const char *Text) { Assign(Text); return *this; }

private:
    StringData m_sStringData;
    char m_strStackBuffer[L + 1];

    inline void InitStackStringData()
    {
        m_sStringData.pBuffer = m_strStackBuffer;
        m_sStringData.StringLength = 0;
        m_sStringData.BufferSize = countof(m_strStackBuffer);
        m_sStringData.ReadOnly = false;
        m_sStringData.ReferenceCount = -1;
        
#if Y_BUILD_CONFIG_DEBUG
        Y_memzero(m_strStackBuffer, sizeof(m_strStackBuffer));
#else
        m_strStackBuffer[0] = '\0';
#endif
    }
};

// stack string types
typedef StackString<64> TinyString;
typedef StackString<256> SmallString;
typedef StackString<512> LargeString;
typedef StackString<512> PathString;

// empty string global
extern const String EmptyString;

