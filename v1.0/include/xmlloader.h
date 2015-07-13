#ifndef LZ_XMLLOADER_H
#define LZ_XMLLOADER_H

/**
 * 兰州项目XML文件解析包含头文件
 *
 * @author fanxiang
 * @version 1.0.0
 * @date 20140114
 */

// TINYXML类库
#include <../module/xmlloader/include/tinyxml/tinystr.h>
#include <../module/xmlloader/include/tinyxml/tinyxml.h>

// 解析父类
#include <../module/xmlloader/include/xmlfileloader.h>
// 子类
#include <../module/xmlloader/include/xmlnetworkfileloader.h>
#include <../module/xmlloader/include/xmlsynthesisheightsfileloader.h>
#include <../module/xmlloader/include/xmltaskfileloader.h>
#include <../module/xmlloader/include/xmlrealtaskfileloader.h>
#include <../module/xmlloader/include/xmlcheckedtaskfileloader.h>

// XML文件在内存中对应存储到数据结构
#include <../module/datastructure/include/slavemodel.h>
#include <../module/datastructure/include/camerasnid.h>
#include <../module/datastructure/include/output_heights_list.h>
#include <../module/datastructure/include/plantask.h>
#include <../module/datastructure/include/plantask_list.h>
#include <../module/datastructure/include/realtask.h>
#include <../module/datastructure/include/realtask_list.h>
#include <../module/datastructure/include/checkedtask.h>
#include <../module/datastructure/include/checkedtask_list.h>

#endif // LZ_XMLLOADER_H
