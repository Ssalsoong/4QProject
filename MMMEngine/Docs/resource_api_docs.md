# Resource API Documentation

## 개요

Resource API는 게임 엔진의 모든 에셋(텍스처, 메시, 머티리얼 등)을 관리하는 시스템입니다. Editor와 Player 환경에서 서로 다른 방식으로 동작하지만, 동일한 인터페이스를 제공합니다.

## 아키텍처 개요

```
┌─────────────────────────────────────────────────────┐
│                  User Code                          │
│         Resources::Load<T>(path)                    │
└──────────────────┬──────────────────────────────────┘
                   │
┌──────────────────▼──────────────────────────────────┐
│              ResourceManager                         │
│  - Load<T>()                                        │
│  - Cache management                                 │
└──────┬───────────────────────────┬──────────────────┘
       │                           │
┌──────▼──────────┐       ┌────────▼─────────────┐
│ IAssetResolver  │       │  IBytesProvider      │
│ (경로→AssetEntry)│       │  (바이너리 읽기)      │
└──────┬──────────┘       └────────┬─────────────┘
       │                           │
   ┌───▼────┬──────────┐      ┌────▼────┬─────────┐
   │ Editor │  Player  │      │ Editor  │ Player  │
   │Resolver│ Resolver │      │FileProv │PakProv  │
   └────────┴──────────┘      └─────────┴─────────┘

Editor: 개별 파일들 (.mtext 등)
Player: 단일 .pak 파일
```

## 핵심 컴포넌트

### 1. Resource (Base Class)

모든 리소스의 기본 클래스입니다.

```cpp
class Resource
{
private:
    Utility::MUID m_muid;
    
protected:
    // 파생 클래스가 구현해야 함
    virtual bool LoadFromBytes(const void* data, size_t size) = 0;
    
public:
    virtual ~Resource() = default;
    const Utility::MUID& GetMUID() const { return m_muid; }
};
```

**주요 메서드:**
- `LoadFromBytes()`: 바이너리 데이터로부터 리소스를 로드
- `GetMUID()`: 리소스의 고유 ID 반환

### 2. AssetEntry

리소스의 위치 정보를 담는 구조체입니다.

```cpp
struct AssetEntry
{
    enum class Source { File, Pak };
    
    Utility::MUID muid;        // 고유 ID
    CacheType cacheType;             // 파일 or Pak
    std::wstring filePath;     // File일 때만 사용
    uint64_t offset;           // Pak일 때만 사용
    uint64_t size;             // Pak일 때만 사용
};
```

### 3. IAssetResolver

경로를 AssetEntry로 변환하는 인터페이스입니다.

```cpp
struct IAssetResolver
{
    virtual bool Resolve(std::string_view sourcePath, AssetEntry& out) const = 0;
    virtual bool Resolve(const Utility::MUID& muid, AssetEntry& out) const = 0;
};
```

### 4. IBytesProvider

AssetEntry로부터 실제 바이너리를 읽는 인터페이스입니다.

```cpp
struct IBytesProvider
{
    virtual bool ReadAll(const AssetEntry& entry, std::vector<uint8_t>& outBytes) = 0;
};
```

## Editor vs Player 구현

### Editor 환경

Editor에서는 개별 파일 시스템을 사용합니다.

#### 디렉토리 구조
```
ProjectRoot/
├── Assets/              # 원본 소스 파일
│   ├── Textures/
│   │   └── logo.png
│   └── Models/
│       └── character.fbx
├── Library/             # Import된 artifact
│   ├── Textures/
│   │   └── logo.mtext   # 변환된 텍스처
│   └── Models/
│       └── character.mmesh
└── Library/Meta/        # 메타데이터
    └── database.json
```

#### Editor 구성 요소

**1. MetaDB**
```cpp
class MetaDB
{
    // sourcePath → MetaFile 매핑
    std::unordered_map<std::string, MetaFile> m_pathMap;
    // MUID → MetaFile 매핑
    std::unordered_map<Utility::MUID, MetaFile> m_muidMap;
    
    bool TryResolve(std::string_view path, MetaFile& out);
    bool TryResolve(const Utility::MUID& muid, MetaFile& out);
};
```

**2. EditorResolver**
```cpp
class EditorResolver : public IAssetResolver
{
    const MetaDB* m_db;
    
    bool Resolve(std::string_view sourcePath, AssetEntry& out) const override
    {
        MetaFile meta{};
        if (!m_db->TryResolve(sourcePath, meta)) 
            return false;
        
        out.muid = meta.muid;
        out.cacheType = AssetEntry::CacheType::File;
        out.filePath = meta.artifactPath;  // "Library/Textures/logo.mtext"
        out.offset = 0;
        out.size = 0;
        return true;
    }
};
```

**3. FileBytesProvider**
```cpp
class FileBytesProvider : public IBytesProvider
{
    bool ReadAll(const AssetEntry& entry, std::vector<uint8_t>& outBytes) override
    {
        if (entry.cacheType != AssetEntry::CacheType::File) 
            return false;
        
        std::ifstream f(entry.filePath, std::ios::binary);
        if (!f) return false;
        
        f.seekg(0, std::ios::end);
        const auto sz = f.tellg();
        f.seekg(0, std::ios::beg);
        
        outBytes.resize((size_t)sz);
        if (sz > 0) f.read((char*)outBytes.data(), sz);
        return true;
    }
};
```

#### Editor 글로벌 레지스트리

```cpp
// EditorRegistry.h
namespace MMMEngine::Editor
{
    inline static MetaDB g_db;
    inline static EditorResolver g_resolver{ &g_db };
    inline static FileBytesProvider g_filesProvider;
    inline static ImporterRegistry g_importerRegistry;
}
```

### Player 환경

Player에서는 모든 리소스가 단일 .pak 파일에 패킹됩니다.

#### 파일 구조
```
GameBuild/
├── Game.exe
├── Game.pak           # 모든 리소스가 하나의 파일에
├── PathMap.bin        # 경로 → MUID 매핑
└── AssetIndex.bin     # MUID → offset/size 매핑
```

#### .pak 파일 포맷
```
┌─────────────────────────────────────┐
│  Asset 1 Data (logo.mtext)          │ offset: 0, size: 1024
├─────────────────────────────────────┤
│  Asset 2 Data (character.mmesh)     │ offset: 1024, size: 5000
├─────────────────────────────────────┤
│  Asset 3 Data (music.maudio)        │ offset: 6024, size: 50000
└─────────────────────────────────────┘
```

#### Player 구성 요소

**1. PathMapRuntime**
```cpp
class PathMapRuntime
{
    std::unordered_map<std::string, Utility::MUID> m_map;
    
    bool TryGetMUID(std::string_view sourcePath, Utility::MUID& out) const;
};

// PathMap.bin 포맷:
// Header: magic(4) | version(4) | entryCount(4)
// Entry: pathLength(2) | path(variable) | muid(16)
```

**2. AssetIndexRuntime**
```cpp
class AssetIndexRuntime
{
    std::unordered_map<Utility::MUID, AssetSpan> m_map;
    
    bool TryGetSpan(const Utility::MUID& muid, AssetSpan& out) const;
};

struct AssetSpan
{
    uint64_t offset;
    uint64_t size;
};

// AssetIndex.bin 포맷:
// Header: magic('AIX0') | version(1) | entryCount(4)
// Entry: muid(16) | offset(8) | size(8)
```

**3. PakFileReader**
```cpp
class PakFileReader
{
    std::ifstream m_file;
    
    bool Open(const std::wstring& pakPath);
    
    bool Read(uint64_t offset, uint64_t size, std::vector<uint8_t>& outBytes)
    {
        outBytes.resize(size);
        m_file.seekg(offset, std::ios::beg);
        if (size > 0)
            m_file.read((char*)outBytes.data(), size);
        return true;
    }
};
```

**4. PlayerResolver**
```cpp
class PlayerResolver : public IAssetResolver
{
    const PathMapRuntime* m_pathMap;
    const AssetIndexRuntime* m_index;
    
    bool Resolve(std::string_view sourcePath, AssetEntry& out) const override
    {
        // 1. 경로 → MUID
        Utility::MUID muid{};
        if (!m_pathMap->TryGetMUID(sourcePath, muid)) 
            return false;
        
        // 2. MUID → offset/size
        AssetSpan sp{};
        if (!m_index->TryGetSpan(muid, sp)) 
            return false;
        
        out.muid = muid;
        out.cacheType = AssetEntry::CacheType::Pak;
        out.offset = sp.offset;
        out.size = sp.size;
        return true;
    }
};
```

**5. PakBytesProvider**
```cpp
class PakBytesProvider : public IBytesProvider
{
    PakFileReader* m_reader;
    
    bool ReadAll(const AssetEntry& entry, std::vector<uint8_t>& outBytes) override
    {
        if (entry.cacheType != AssetEntry::CacheType::Pak) 
            return false;
        return m_reader->Read(entry.offset, entry.size, outBytes);
    }
};
```

#### Player 글로벌 레지스트리

```cpp
// PlayerRegistry.h
namespace MMMEngine::Player
{
    inline static PathMapRuntime g_pathMap;
    inline static AssetIndexRuntime g_assetIndex;
    inline static PakFileReader g_pak;
    inline static PlayerResolver g_resolver{ &g_pathMap, &g_assetIndex };
    inline static PakBytesProvider g_bytes{ &g_pak };
}
```

## ResourceManager 사용법

### 기본 로딩

```cpp
// 경로로 로딩
auto texture = Resources::Load<Texture2D>(L"Assets/Textures/logo.png");

// MUID로 로딩 (고급)
Utility::MUID muid = /* ... */;
auto mesh = ResourceManager::Get().LoadByMUID<Mesh>(muid);
```

### ResPtr (스마트 포인터)

```cpp
ResPtr<Texture2D> m_logoTexture;

void Initialize()
{
    // 로딩
    m_logoTexture = Resources::Load<Texture2D>(L"Assets/Textures/logo.png");
    
    // 유효성 검사
    if (m_logoTexture)
    {
        int width = m_logoTexture->GetWidth();
    }
}

// ResPtr은 자동으로 참조 카운트 관리
```

### 내부 동작 흐름

```cpp
// Resources::Load<Texture2D>(L"Assets/Textures/logo.png") 호출 시:

1. ResourceManager::Load<Texture2D>()
   ↓
2. 캐시 확인: m_cache에 이미 로드되어 있는가?
   ↓ (없으면)
3. IAssetResolver::Resolve()
   - Editor: EditorResolver → MetaDB 조회
   - Player: PlayerResolver → PathMap + AssetIndex 조회
   ↓
4. AssetEntry 획득 (muid, source, offset/size or filePath)
   ↓
5. IBytesProvider::ReadAll()
   - Editor: FileBytesProvider → 파일 읽기
   - Player: PakBytesProvider → .pak에서 offset 기준 읽기
   ↓
6. std::vector<uint8_t> 바이너리 획득
   ↓
7. Resource::LoadFromBytes()
   - Texture2D: 이미지 파싱, GPU 업로드
   - Mesh: 버텍스/인덱스 버퍼 생성
   ↓
8. ResPtr 반환 및 캐시 저장
```

## 리소스 타입 정의하기

### 1. Resource 클래스 작성

```cpp
// Texture2D.h
class Texture2D : public Resource
{
    RTTR_ENABLE(Resource)
    RTTR_REGISTRATION_FRIEND
    
private:
    int m_width = 0;
    int m_height = 0;
    int m_channels = 0;
    std::vector<uint8_t> m_pixels;
    // ID3D11Texture2D* m_texture = nullptr;  // DirectX 텍스처
    
protected:
    bool LoadFromBytes(const void* data, size_t size) override
    {
        // 1. 바이너리를 파싱 (.mtext 포맷)
        // 2. GPU 텍스처 생성
        // 3. 멤버 변수에 저장
        return true;
    }
    
public:
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
};
```

### 2. RTTR 등록

```cpp
// Texture2D.cpp
RTTR_REGISTRATION
{
    using namespace rttr;
    registration::class_<Texture2D>("Texture2D")
        .constructor<>()
        .property("width", &Texture2D::GetWidth)
        .property("height", &Texture2D::GetHeight);
}
```

## Editor 초기화 (Editor 프로젝트)

```cpp
// EditorMain.cpp
#include "EditorRegistry.h"

int main()
{
    using namespace MMMEngine::Editor;
    
    // 1. MetaDB 로드
    g_db.LoadFromFile(L"Library/Meta/database.json");
    
    // 2. ResourceManager 설정
    ResourceManager::Get().SetResolver(&g_resolver);
    ResourceManager::Get().SetBytesProvider(&g_filesProvider);
    
    // 3. 리소스 사용 가능
    auto logo = Resources::Load<Texture2D>(L"Assets/Textures/logo.png");
    
    return 0;
}
```

## Player 초기화 (Player 프로젝트)

```cpp
// PlayerMain.cpp
#include "PlayerRegistry.h"

int WinMain()
{
    using namespace MMMEngine::Player;
    
    // 1. 런타임 데이터 로드
    g_pathMap.LoadFromFile(L"PathMap.bin");
    g_assetIndex.LoadFromFile(L"AssetIndex.bin");
    g_pak.Open(L"Game.pak");
    
    // 2. ResourceManager 설정
    ResourceManager::Get().SetResolver(&g_resolver);
    ResourceManager::Get().SetBytesProvider(&g_bytes);
    
    // 3. 리소스 사용 가능 (Editor와 동일한 코드!)
    auto logo = Resources::Load<Texture2D>(L"Assets/Textures/logo.png");
    
    return 0;
}
```

## 빌드 파이프라인 (Editor → Player)

### 1. Import 단계 (Editor)

```cpp
// Importer 인터페이스
struct IImporter
{
    virtual bool Import(const ImportContext& ctx) = 0;
    virtual std::string GetName() const = 0;
};

struct ImportContext
{
    std::wstring sourcePath;      // Assets/Textures/logo.png
    std::wstring artifactPath;    // Library/Textures/logo.mtext
    Utility::MUID muid;
    AssetType type;
};

// 예: TextureImporter
class TextureImporter : public IImporter
{
    bool Import(const ImportContext& ctx) override
    {
        // 1. PNG 파일 로드
        // 2. .mtext 포맷으로 변환 (압축, 최적화 등)
        // 3. artifactPath에 저장
        return true;
    }
};
```

### 2. Build 단계 (Editor → .pak 생성)

```cpp
// PakBuilder.cpp (Editor 전용)
class PakBuilder
{
    void Build(const std::wstring& outputPakPath)
    {
        std::ofstream pak(outputPakPath, std::ios::binary);
        std::vector<AssetIndexEntry> index;
        std::unordered_map<std::string, Utility::MUID> pathMap;
        
        uint64_t currentOffset = 0;
        
        // MetaDB의 모든 artifact를 pak에 추가
        for (const auto& meta : g_db.GetAllMetas())
        {
            // 1. artifact 파일 읽기
            std::ifstream artifact(meta.artifactPath, std::ios::binary);
            std::vector<uint8_t> data(...);
            
            // 2. pak에 쓰기
            pak.write((char*)data.data(), data.size());
            
            // 3. 인덱스 기록
            index.push_back({
                .muid = meta.muid,
                .offset = currentOffset,
                .size = data.size()
            });
            
            // 4. PathMap 기록
            pathMap[meta.sourcePath] = meta.muid;
            
            currentOffset += data.size();
        }
        
        // PathMap.bin 저장
        SavePathMap(pathMap, L"PathMap.bin");
        
        // AssetIndex.bin 저장
        SaveAssetIndex(index, L"AssetIndex.bin");
    }
};
```

## 실전 예제

### 게임 시작 시 로딩

```cpp
class Game
{
    ResPtr<Texture2D> m_playerTexture;
    ResPtr<Mesh> m_playerMesh;
    ResPtr<Material> m_playerMaterial;
    
    void LoadAssets()
    {
        m_playerTexture = Resources::Load<Texture2D>(
            L"Assets/Characters/Player/player_diffuse.png"
        );
        
        m_playerMesh = Resources::Load<Mesh>(
            L"Assets/Characters/Player/player.fbx"
        );
        
        m_playerMaterial = Resources::Load<Material>(
            L"Assets/Materials/PlayerMaterial.mat"
        );
    }
};
```

### Scene 파일에서 참조

```cpp
// Scene 저장 시
void SceneSerializer::SaveMaterial(Material* mat)
{
    json j;
    j["muid"] = mat->GetMUID().ToString();
    j["diffuseTexture"] = mat->GetDiffuseTexture()->GetMUID().ToString();
    // ...
}

// Scene 로드 시
Material* SceneSerializer::LoadMaterial(const json& j)
{
    auto muid = Utility::MUID::Parse(j["muid"]);
    auto mat = ResourceManager::Get().LoadByMUID<Material>(muid);
    
    auto texMuid = Utility::MUID::Parse(j["diffuseTexture"]);
    auto tex = ResourceManager::Get().LoadByMUID<Texture2D>(texMuid);
    mat->SetDiffuseTexture(tex);
    
    return mat;
}
```

## 디버깅 팁

### Editor에서 리소스 찾기 실패

```cpp
// 1. MetaDB에 등록되어 있는지 확인
if (!g_db.Contains("Assets/Textures/logo.png"))
{
    // Import가 필요함
    g_importerRegistry.Import("Assets/Textures/logo.png");
}

// 2. artifact 파일이 존재하는지 확인
MetaFile meta;
g_db.TryResolve("Assets/Textures/logo.png", meta);
if (!std::filesystem::exists(meta.artifactPath))
{
    // Re-import 필요
}
```

### Player에서 리소스 찾기 실패

```cpp
// 1. PathMap에 등록되어 있는지
Utility::MUID muid;
if (!g_pathMap.TryGetMUID("Assets/Textures/logo.png", muid))
{
    // 빌드 시 누락됨 - Editor에서 재빌드 필요
}

// 2. AssetIndex에 등록되어 있는지
AssetSpan span;
if (!g_assetIndex.TryGetSpan(muid, span))
{
    // 인덱스 손상 - 재빌드 필요
}

// 3. .pak 파일이 올바른지
PakFileReader reader;
reader.Open(L"Game.pak");
std::vector<uint8_t> data;
if (!reader.Read(span.offset, span.size, data))
{
    // .pak 파일 손상
}
```

## 성능 고려사항

### 캐싱
- ResourceManager는 로드된 모든 리소스를 MUID 기준으로 캐싱
- 같은 리소스를 여러 번 로드해도 실제로는 한 번만 메모리에 적재

### 비동기 로딩 (향후 확장)
```cpp
// 현재는 동기 로딩만 지원
auto tex = Resources::Load<Texture2D>(path);

// 향후 비동기 API
auto handle = Resources::LoadAsync<Texture2D>(path);
// ... 다른 작업
if (handle.IsReady())
{
    auto tex = handle.Get();
}
```

### Player 최적화
- .pak 파일은 SSD에서 매우 빠름 (단일 파일 I/O)
- 메모리 매핑 사용 고려 가능
- LZ4 등 고속 압축 추가 가능

## 요약

| 항목 | Editor | Player |
|------|--------|--------|
| **Resolver** | EditorResolver | PlayerResolver |
| **BytesProvider** | FileBytesProvider | PakBytesProvider |
| **저장소** | 개별 파일 (Library/) | 단일 .pak |
| **메타데이터** | MetaDB (JSON) | PathMap + AssetIndex (Binary) |
| **사용 코드** | 동일 (Resources::Load) | 동일 |
| **빌드 타겟** | Editor.exe | Game.exe + Game.pak |

---

**핵심 원칙:**
1. Editor와 Player는 동일한 Resource API 사용
2. Resolver/Provider만 교체하여 다른 저장 방식 지원
3. MUID로 모든 리소스 추적 → 경로 변경에 안전
