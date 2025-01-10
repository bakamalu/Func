#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <QFile>

using namespace std;

#define _Project  "Project"
#define _EndProject  "EndProject"
#define _ProjectSection  "ProjectSection(ProjectDependencies) = postProject"
#define _EndProjectSection  "EndProjectSection"
#define _GlobalSection  "GlobalSection(NestedProjects)"
#define _EndGlobalSection  "EndGlobalSection"
#define _vcxProjectReference  "<ProjectReference"
#define _vcxEndProjectReference  "</ProjectReference>"
#define _vcxProject  "<Project>"
#define _vcxEndProject "</Project>"

#define _vcxClCompile "<ClCompile Include"
#define _vcxEndClCompile ">"
#define _vcxClInclude "<ClInclude Include"
#define _vcxEndClInclude ">"


class Project;
Project* getProjectByName(string strName);
Project* getProject(string strGuid, string strName = "", string strPath = "");

string toUpper(const string& str);
bool parseStr(string& strSrcInfo, string& strParse, const char* strBegin, const char* strEnd);
bool containCompeleteStr(const string& strSrc, const string& strfind);

class ProjectGraphNode
{
public:
    ProjectGraphNode()
        : m_nDepth(0) {}

    ProjectGraphNode(string strProjectName)
        : m_strProjectName(strProjectName)
        , m_nDepth(0) {}

    long long depth();
public:
    string                      m_strProjectName;
    set<ProjectGraphNode*>      m_setChild;
    set<ProjectGraphNode*>      m_setParent;
    long long                   m_nDepth;
};

class Project {

public:
    Project() {}
    Project(string name, string path, string guid, Project* parent = NULL);

    void showLevel(int nLevel);
    void clean();

    void getParents(int nLevel, set<Project*>& setProjectNameInGraph);

    void setParent(Project* parent);

public:
    string m_strName;
    string m_strPath;
    string m_strGuid;

    set<string>                         m_setIncludePath;
    set<string>                         m_setCompilePath;
    set<string>                         m_setPossibleRedundant;
    static map<string, Project*>        s_mapProject;
    map<string, ProjectGraphNode*>      m_mapGraphNode;
    set<Project*> m_setParent;

private:
    long long m_nUseCount = 0;
};

class ProjectAnalyze
{
public:
    ProjectAnalyze(string strPath, bool bLoadCl);

protected:
    void parseSlnFile(const string& strPath);
    void parseSlnFileLine(QFile& slnFile, string& strCurInfo, string strPath);
    void parseVcxprojFile(Project* pProject, const string& strPath);
    void parseVcxprojFileLine(Project* pProject, QFile& vcxFile, string& strCurInfo, string strDirectoryPath);

    void parseProject(string& strInfo, string strPath);
    void parseGlobalSection(string& strInfo);
    void parseVcxProjectReference(Project* pProject, string& strInfo, string strDirectoryPath);
    void parseVcxProject(string& strInfo, string& strGuid);
    void parseVcxProjectCl(Project* pProject, string& strInfo, string strDirectoryPath);
    void parseVcxIncludeName(string& strInfo, string& strName);
    void parseVcxCompileName(string& strInfo, string& strName);

private:
    void analyzeReductdant();
    void doAnalyzeReductdant(Project* pProject, const string& strFilePath);
    void parseProjectSection(string& strInfo, vector<string>& vecGuid);

private:
    bool            m_bLoadCl;
    set<string>     m_setVcxprojFilePath;
    set<string>     m_setVcxprojFileName;
    set<string>     m_setNotOpenFileName;
    string          m_strPath;
    Project         * m_pRoot;
};