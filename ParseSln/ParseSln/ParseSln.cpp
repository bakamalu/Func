#include "ParseSln.h"
#include <regex>
#include <filesystem>
#include <QTextStream>

map<string, Project*> Project::s_mapProject;
Project* getProjectByName(string strName)
{
    Project* pProject;
    string strGuid;
    strName = toUpper(strName);
    for (auto& [strGuid, pProject] : Project::s_mapProject)
    {
        if (NULL != pProject && toUpper(pProject->m_strName) == strName)
        {
            return pProject;
        }
    }
    return NULL;
}

Project* getProject(string strGuid, string strName, string strPath )
{
    Project* pProject = NULL;

    strGuid = toUpper(strGuid);
    auto iter = Project::s_mapProject.find(strGuid);
    if (iter == Project::s_mapProject.end())
    {
        pProject = new Project(strName, strPath, strGuid);
        Project::s_mapProject[strGuid] = pProject;
    }
    else
    {
        pProject = iter->second;
        if (!strName.empty())
        {
            pProject->m_strName = strName;
        }
        if (!strPath.empty())
        {
            pProject->m_strPath = strPath;
        }
    }
    return pProject;
}

string toUpper(const string& str)
{
    string strTemp = str;
    for (char& ch : strTemp)
    {
        if (ch <= 'z' && ch >= 'a')
        {
            ch += ('Z' - 'z');
        }
    }
    return strTemp;
}

bool parseStr(string& strSrcInfo, string& strParse, const char* strBegin, const char* strEnd)
{
    int nStart = strSrcInfo.find(strBegin);
    int nEnd = strSrcInfo.find(strEnd, nStart) + strlen(strEnd);

    if (nStart < 0 || nEnd < 0)
    {
        return false;
    }
    strParse = strSrcInfo.substr(nStart, nEnd - nStart + 1);
    strSrcInfo.erase(strSrcInfo.begin() + nStart, strSrcInfo.begin() + nEnd);
    return true;
}

bool containCompeleteStr(const string& strSrc, const string& strfind)
{
    int nIndexFind = 0;
    nIndexFind = strSrc.find(strfind, nIndexFind);
    while (nIndexFind > -1)
    {
        //cout << nIndexFind << endl;
        nIndexFind += strfind.size();
        //cout << nIndexFind << endl;
        if (nIndexFind < strSrc.size())
        {
            char chEnd = strSrc[nIndexFind];
            if ((chEnd >= 'a' && chEnd <= 'z') || (chEnd >= 'A' && chEnd <= 'Z'))
            {
                //cout << "not end";
            }
            else
            {
                //cout << "not letter";
                return true;
            }
        }
        else
        {
            //cout << "over";
            return true;
        }
        nIndexFind = strSrc.find(strfind, nIndexFind);
    }
    return false;
}

////////////////////////ProjectGraphNode///////////////////////////////////////////////////
long long ProjectGraphNode::depth()
{
    // 1. 关联项目数
    //return m_setChild.size() + m_setParent.size()
    if (m_nDepth == 0)
    {
        m_nDepth = 2;
        long long nTempDepth = 0;
        for (ProjectGraphNode* pParentNode : m_setParent)
        {
            nTempDepth += m_nDepth * pParentNode->depth();
        }
        m_nDepth = m_nDepth + nTempDepth;
    }
    return m_nDepth;
}

////////////////////////Project///////////////////////////////////////////////////
Project::Project(string name, string path, string guid, Project* parent /*= NULL*/)
    : m_strName(name)
    , m_strPath(path)
    , m_strGuid(guid)
{
    setParent(parent);
}

void Project::showLevel(int nLevel)
{
    clean();

    set<Project* > setProjectInGraph;
    getParents(nLevel, setProjectInGraph);

    map<string, ProjectGraphNode*>mapGraphNode;
    auto getPrahNode = [this](string strProjectName) {
        auto iter = m_mapGraphNode.find(strProjectName);
        if (iter == m_mapGraphNode.end())
        {
            ProjectGraphNode* pNode = new ProjectGraphNode(strProjectName);
            m_mapGraphNode[strProjectName] = pNode;
            return pNode;
        }
        else
        {
            return iter->second;
        }
    };
    for (Project* pProject : setProjectInGraph)
    {
        ProjectGraphNode* pNode = getPrahNode(pProject->m_strName);
        for (Project* pChildProject : setProjectInGraph)
        {
            if (pChildProject->m_setParent.find(pProject) != pChildProject->m_setParent.end())
            {
                string strChildName = pChildProject->m_strName;
                pNode->m_setChild.insert(getPrahNode(strChildName));
                getPrahNode(strChildName)->m_setParent.insert(pNode);
            }
        }
    }
}

void Project::clean()
{
    // 清理
    for (auto iter : m_mapGraphNode)
    {
        ProjectGraphNode* pNode = iter.second;
        if (pNode)
        {
            delete pNode;
            pNode = NULL;
        }
    }
    m_mapGraphNode.clear();
}

void Project::getParents(int nLevel, set<Project*>& setProjectInGraph)
{
    if (nLevel == 0)
    {
        return;
    }
    if (m_strName.empty())
    {
        return;
    }
    if (setProjectInGraph.find(this) == setProjectInGraph.end())
    {
        setProjectInGraph.insert(this);
        for (Project* pParent : m_setParent)
        {
            pParent->getParents(nLevel - 1, setProjectInGraph);
        }
    }
}

void Project::setParent(Project* parent)
{
    if (parent)
    {
        //parent->m_setChild.insert(this);
        m_setParent.insert(parent);
    }
    ++m_nUseCount;
}

////////////////////////ProjectAnalyze///////////////////////////////////////////////////
ProjectAnalyze::ProjectAnalyze(string strPath, bool bLoadCl)
    : m_strPath(strPath)
    , m_bLoadCl(bLoadCl)
    , m_pRoot(new Project("", m_strPath, "", NULL))
{
    parseSlnFile(strPath);

    for (const string& strFileName : m_setNotOpenFileName)
    {
        if (m_setVcxprojFileName.find(strFileName) == m_setVcxprojFileName.end())
        {
            cout << "not open file : " << strFileName << endl;
        }
    }
}

void ProjectAnalyze::parseSlnFile(const string& strPath)
{
    QFile slnFile(QString::fromStdString(strPath));
    if (!slnFile.open(QIODevice::ReadOnly | QIODevice::Text) )
    {
        cerr << "无法打开"<< strPath  <<"文件" << endl;
        return;
    }
    string strInfo;
    parseSlnFileLine(slnFile, strInfo, strPath);
    analyzeReductdant();
}

bool getLine(QTextStream& in, std::string& strLine)
{
    QString line = in.readLine();
    strLine = line.toStdString();

    return !line.isNull();
}

void ProjectAnalyze::parseSlnFileLine(QFile& slnFile, string& strCurInfo, string strPath)
{
    string strLine;
    QTextStream in(&slnFile);
    while (getLine(in, strLine))
    {
        strCurInfo += strLine;
        if (strCurInfo.find(_Project) != string::npos)
        {
            bool bEndProject = false;
            while (!bEndProject && getLine(in, strLine))
            {
                strCurInfo += strLine;
                if (containCompeleteStr(strCurInfo, _EndProject)) {
                    bEndProject = true;
                    parseProject(strCurInfo, strPath);
                    while (containCompeleteStr(strCurInfo, _EndProject))
                    {
                        parseProject(strCurInfo, strPath);
                    }
                }
            }
        }
        else if (strCurInfo.find(_GlobalSection) != string::npos)
        {
            bool bEndGlobalSection = false;
            while (!bEndGlobalSection && getLine(in, strLine))
            {
                strCurInfo += strLine;
                if (containCompeleteStr(strCurInfo, _EndGlobalSection)) {
                    bEndGlobalSection = true;
                    parseGlobalSection(strCurInfo);
                    while (containCompeleteStr(strCurInfo, _EndGlobalSection))
                    {
                        parseGlobalSection(strCurInfo);
                    }
                }
            }
        }
        else
        {
            strCurInfo.clear();
        }
    }
}

string getDirectoryPath(const string& strPath)
{
    string strPathTemp = strPath;
    std::replace(strPathTemp.begin(), strPathTemp.end(), '\\', '/');
    int nLastDirectory = strPathTemp.rfind('/') + 1;
    const string& strFileName = strPathTemp.substr(0, nLastDirectory);
    return strFileName;
}

string getFileNameFromPath(const string& strPath)
{
    string strPathTemp = strPath;
    std::replace(strPathTemp.begin(), strPathTemp.end(), '\\', '/');
    int nLastDirectory = strPathTemp.rfind('/') + 1;
    const string& strFileName = strPathTemp.substr(nLastDirectory);
    return strFileName;
}

void ProjectAnalyze::parseVcxprojFile(Project* pProject, const string& strPath)
{
    QFile vcxFile(QString::fromStdString(strPath));
    if (!vcxFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        m_setNotOpenFileName.insert(getFileNameFromPath(strPath));
        return;
    }
    string strRealPath = std::filesystem::absolute(strPath).generic_string();
    if (m_setVcxprojFilePath.find(strRealPath) == m_setVcxprojFilePath.end())
    {
        m_setVcxprojFileName.insert(getFileNameFromPath(strRealPath));
        m_setVcxprojFilePath.insert(strRealPath);
        pProject->m_strPath = strRealPath;

        string strInfo;
        parseVcxprojFileLine(pProject, vcxFile, strInfo, getDirectoryPath(strRealPath));
    }
}

void ProjectAnalyze::parseVcxprojFileLine(Project* pProject, QFile& vcxFile, string& strCurInfo, string strDirectoryPath)
{
    string strLine;
    QTextStream in(&vcxFile);
    while (getLine(in, strLine))
    {
        strCurInfo += strLine;
        if (strCurInfo.find(_vcxProjectReference) != string::npos)
        {
            bool bEndProject = false;
            while (!bEndProject && getLine(in, strLine))
            {
                strCurInfo += strLine;
                if (containCompeleteStr(strCurInfo, _vcxEndProjectReference)) {
                    bEndProject = true;
                    parseVcxProjectReference(pProject, strCurInfo, strDirectoryPath);
                    while (containCompeleteStr(strCurInfo, _vcxEndProjectReference))
                    {
                        parseVcxProjectReference(pProject, strCurInfo, strDirectoryPath);
                    }
                }
            }
        }
        else if (m_bLoadCl && (strCurInfo.find(_vcxClCompile) != string::npos || strCurInfo.find(_vcxClInclude) != string::npos))
        {
            bool bEndItemGroup = false;
            while (!bEndItemGroup)
            {
                if (containCompeleteStr(strCurInfo, _vcxEndClCompile)) {
                    bEndItemGroup = true;
                    parseVcxProjectCl(pProject, strCurInfo, strDirectoryPath);
                    while (containCompeleteStr(strCurInfo, _vcxEndClCompile))
                    {
                        parseVcxProjectCl(pProject, strCurInfo, strDirectoryPath);
                    }
                }
                else if (containCompeleteStr(strCurInfo, _vcxEndClInclude)) {
                    bEndItemGroup = true;
                    parseVcxProjectCl(pProject, strCurInfo, strDirectoryPath);
                    while (containCompeleteStr(strCurInfo, _vcxEndClInclude))
                    {
                        parseVcxProjectCl(pProject, strCurInfo, strDirectoryPath);
                    }
                }
                else
                {
                    if (!getLine(in, strLine))
                    {
                        break;
                    }
                    strCurInfo += strLine;
                }
            }
        }
        else
        {
            strCurInfo.clear();
        }
    }
}

void ProjectAnalyze::parseProject(string& strInfo, string strPath)
{
    vector<string>parentGuid;
    if (strInfo.find(_ProjectSection) != string::npos)
    {
        while (strInfo.find(_EndProjectSection) != string::npos)
        {
            parseProjectSection(strInfo, parentGuid);
        }
    }

    string strMess;
    if (!parseStr(strInfo, strMess, _Project, _EndProject))
    {
        return;
    }

    std::regex pattern(R"(Project\(\"\{([^}]+)\}\"\) = \"([^"]+)\", \"([^"]+)\", \"\{([^}]+)\}\")");

    // 用于存储匹配结果
    std::smatch match;

    Project* curProject = NULL;
    // 使用std::regex_search尝试找到匹配项
    if (std::regex_search(strMess, match, pattern)) {
        // 提取匹配到的内容
        string strChildGuid = match[1].str();
        string strProjectName = match[2].str();
        string strProjectPath = match[3].str();
        string strProjectGuid = match[4].str();

        Project* childProject = getProject(strChildGuid);
        curProject = getProject(strProjectGuid, strProjectName, strProjectPath);
        childProject->setParent(curProject);

        if (strProjectPath.find(".vcxproj") != string::npos)
        {
            string strDirectoryPath = getDirectoryPath(strPath);
            string strRealProjectPath = strDirectoryPath + strProjectPath;
            std::replace(strRealProjectPath.begin(), strRealProjectPath.end(), '\\', '/');
            parseVcxprojFile(curProject, strRealProjectPath);
        }
    }
    else {
        std::cout << "No match found for str : " << strMess << std::endl;
    }
    if (!parentGuid.empty())
    {
        for (string guid : parentGuid)
        {
            Project* parentProject = getProject(guid);
            curProject->setParent(parentProject);
        }
    }
}

void ProjectAnalyze::parseGlobalSection(string& strInfo)
{
    string strMess;
    if (!parseStr(strInfo, strMess, _GlobalSection, _EndGlobalSection))
    {
        return;
    }

    std::regex pattern(R"(\{([^}]+)\} = \{([^}]+)\})");  // 定义正则表达式模式

    std::sregex_iterator it(strMess.begin(), strMess.end(), pattern);  // 迭代器用于遍历匹配结果
    std::sregex_iterator end;  // 结束迭代器
    while (it != end) {
        std::smatch match = *it;
        std::string guidChild = match[1].str();
        std::string guidParent = match[2].str();

        getProject(guidChild)->setParent(getProject(guidParent));
        ++it;
    }
}

void ProjectAnalyze::parseVcxProjectReference(Project* pProject, string& strInfo, string strDirectoryPath)
{
    string strGuid;
    if (strInfo.find(_vcxProject) != string::npos)
    {
        while (strInfo.find(_vcxProject) != string::npos)
        {
            parseVcxProject(strInfo, strGuid);
        }
    }

    string strMess;
    if (!parseStr(strInfo, strMess, _vcxProjectReference, _vcxEndProjectReference))
    {
        return;
    }

    std::regex pattern(R"(Include[^\"]+\"([^\"]+)\">)");
    std::smatch match;

    string strPath;
    // 使用std::regex_search尝试找到匹配项
    if (std::regex_search(strMess, match, pattern)) {
        strPath = match[1].str();  // 第一个捕获组中的内容
        strPath = strDirectoryPath + strPath;
    }
    else {
        string strPathTrim;
        for (char& ch : strPath)
        {
            if (ch != ' ')
            {
                strPathTrim += ch;
            }
        }
        if (!strPathTrim.empty())
        {
            std::cout << "No match strPath found for " << strPath << std::endl;
        }
    }
    Project* pParent = NULL;
    if (!strGuid.empty())
    {
        pParent = getProject(strGuid, "", strPath);
    }
    pProject->setParent(pParent);
    parseVcxprojFile(pParent, strPath);
}

void ProjectAnalyze::parseVcxProject(string& strInfo, string& strGuid)
{
    string strMess;
    if (!parseStr(strInfo, strMess, _vcxProject, _vcxEndProject))
    {
        cout << "parseStr for strInfo fail :"<< strInfo << endl;
        return;
    }

    std::regex pattern(R"(<Project>\{([^\}]+)\}</Project>)");
    std::smatch match;

    // 使用std::regex_search尝试找到匹配项
    if (std::regex_search(strMess, match, pattern)) {
        strGuid = match[1].str();  // 第一个捕获组中的内容
    }
    else {
        std::cout << "No match strGuid found for "<< strGuid << std::endl;
    }
}

void ProjectAnalyze::parseVcxProjectCl(Project* pProject, string& strInfo, string strDirectoryPath)
{
    string name;
    set<string> setFilesPath;
    if (strInfo.find(_vcxClCompile) != string::npos)
    {
        while (strInfo.find(_vcxClCompile) != string::npos)
        {
            parseVcxCompileName(strInfo, name);
            if (!name.empty())
            {
                string strPath = strDirectoryPath + name;
                pProject->m_setCompilePath.insert(strPath);
            }
        }
    }

    if (strInfo.find(_vcxClInclude) != string::npos)
    {
        while (strInfo.find(_vcxClInclude) != string::npos)
        {
            parseVcxIncludeName(strInfo, name);
            if (!name.empty())
            {
                string strPath = strDirectoryPath + name;
                pProject->m_setIncludePath.insert(strPath);
            }
        }
    }
}

void ProjectAnalyze::parseVcxIncludeName(string& strInfo, string& strName)
{
    string strMess;
    if (!parseStr(strInfo, strMess, _vcxClInclude, _vcxEndClInclude))
    {
        cout << "parseStr for strInfo fail :" << strInfo << endl;
        return;
    }

    std::regex pattern(R"(Include[^\"]+"([^\"]+)\")");
    std::smatch match;

    // 使用std::regex_search尝试找到匹配项
    if (std::regex_search(strMess, match, pattern)) {
        strName = match[1].str();  // 第一个捕获组中的内容
    }
    else {
        std::cout << "No match strName found for " << strName << std::endl;
    }
}

void ProjectAnalyze::parseVcxCompileName(string& strInfo, string& strName)
{
    string strMess;
    if (!parseStr(strInfo, strMess, _vcxClCompile, _vcxEndClCompile))
    {
        cout << "parseStr for strInfo fail :" << strInfo << endl;
        return;
    }

    std::regex pattern(R"(Include[^\"]+"([^\"]+)\")");
    std::smatch match;

    // 使用std::regex_search尝试找到匹配项
    if (std::regex_search(strMess, match, pattern)) {
        strName = match[1].str();  // 第一个捕获组中的内容
    }
    else {
        std::cout << "No match strGuid found for " << strName << std::endl;
    }
}

void ProjectAnalyze::analyzeReductdant()
{
    for (auto& iter : Project::s_mapProject)
    {
        Project* pProject = iter.second;
        for (Project* pParent : pProject->m_setParent)
        {
            string strParentName = pParent->m_strName;
            if (!strParentName.empty())
            {
                pProject->m_setPossibleRedundant.insert(strParentName);
            }
        }

        for (auto setPaths : { pProject->m_setCompilePath, pProject->m_setIncludePath })
        {
            for (const string& strClPath : setPaths)
            {
                if (pProject->m_setPossibleRedundant.empty())
                {
                    break;
                }
                doAnalyzeReductdant(pProject, strClPath);
            }
        }
    }
}

void ProjectAnalyze::doAnalyzeReductdant(Project* pProject, const string& strFilePath)
{
    QFile clFile(QString::fromStdString(strFilePath));
    if (!clFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        cout << "not open file " << strFilePath << endl;
        return;
    }
    string strLine;
    QTextStream in(&clFile);
    auto& setRedundant = pProject->m_setPossibleRedundant;
    while (!setRedundant.empty() && getLine(in, strLine))
    {
        // 使用拷贝的迭代器进行遍历和删除
        for (auto it = setRedundant.begin(); it != setRedundant.end(); ) {
            string strProjectName = *it;
            if (strLine.find(*it) != string::npos)
            {
                it = setRedundant.erase(it);
            }
            else
            {
                ++it;
            }
        }
    }

}

void ProjectAnalyze::parseProjectSection(string& strInfo, vector<string>& vecGuid)
{
    string strMess;
    if (!parseStr(strInfo, strMess, _ProjectSection, _EndProjectSection))
    {
        return;
    }

    std::regex pattern(R"(\{([^}]+)\} = \{([^}]+)\})");  // 定义正则表达式模式

    std::sregex_iterator it(strMess.begin(), strMess.end(), pattern);  // 迭代器用于遍历匹配结果
    std::sregex_iterator end;  // 结束迭代器
    while (it != end) {
        std::smatch match = *it;
        if (match[1] == match[2])
        {
            vecGuid.push_back(match[1]);
        }
        else
        {
            cerr << "disEqual " << match[1] << " with " << match[2] << endl;
        }
        ++it;
    }
}