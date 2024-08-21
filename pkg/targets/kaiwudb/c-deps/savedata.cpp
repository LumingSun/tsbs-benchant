//
// Created by hxy on 5/23/23.
//
#include <KWDBConnection.h>
#include <stdlib.h>
#include <mutex>
#include <regex.h>
#include <regex>
#include "savedata.h"

kwdbts::KWDBErrorPtr gKWDBErr = nullptr;
std::shared_ptr<kwdbts::KWDBConnection> KWDBConn = nullptr;
kwdbts::ErrorCode errCode;
int Connect(const char* url, const char* user, const char* passwd){

    // 创建连接
    errCode = kwdbts::KWDBConnection::connect(url, user, passwd, &KWDBConn);
    if (KWDBConn == nullptr) {
        std::cout << "Failed Connect to KWDB, error = " << errCode << "\n";
        return 1;
    }
    //std::cout<<"connect success"<<std::endl;
    return 0;
}

int GetErrObject() {
    // 获取错误对象，用于操作失败后查看错误信息
    gKWDBErr = KWDBConn->getKWDBError();
    if (gKWDBErr == nullptr) {
        //std::cout << "Cannot get KWDBError, may miss detailed error meesage\n";
        return 1;
    }
    return 0;
}

int SetPortal(const char* tenant_name, const char* portal_name){
    errCode = KWDBConn->setCurTenant(tenant_name);
    if (errCode != "00000") {
        std::cout << "failed set Tenant, error = " << errCode << "\n";
        return 1;
    }
    //std::cout << "success to set tenant " << tenant_name << "\n";
    KWDBConn->setCurPortal(portal_name);
    if (errCode != "00000") {
        std::cout << "failed set portal, error = " << errCode << "\n";

        return 1;
    }
    //std::cout << "success to set portal " << portal_name << "\n";
    return 0;
}

kwdbts::KWDBPortalPtr portal = nullptr;
int GetCurPortal() {
    errCode = KWDBConn->getCurPortal(&portal);
    if (portal == nullptr) {
        std::cout << "failed get portal, error = " << errCode << "\n";
        return 1;
    }
    return 0;
}

std::shared_ptr<kwdbts::KWDBDevice> device = nullptr;
int GetDevice(const char* device_name){
    errCode = portal->lookupDevice(device_name, &device);
    if (device == nullptr) {
        std::cout << "failed get device, error = " << errCode << "\n";
        return 1;
    }
    return 0;
}

std::vector<std::string> stringSplit(const std::string& str, const std::regex reg_) {
    std::vector<std::string> elems(std::sregex_token_iterator(str.begin(), str.end(), reg_, -1),
                                   std::sregex_token_iterator());
    return elems;
}

int SaveData(char* data) {

    //string转结构体
    std::string data_string(data, strlen(data));
    const std::regex reg_(",");
    auto csv_row = stringSplit(data, reg_);
    if (csv_row.size() < 2) {
        std::cout<<"Data has No device_name header ";
        return 1;
    }

    // 保存数据之前需要创建Tuples对象用于缓存数据
    kwdbts::TuplesPtr tuples = nullptr;

    //string转结构体
    std::vector<std::any> data1;
    std::string portal_name = portal->getPortalName();
    //todo: （hxy）devops表结构
    if (portal_name.find("readings_") != std::string::npos) {
        for (int i = 0; i < csv_row.size(); ++i) {
            if (i == 0) {
                data1.emplace_back(static_cast<kwdbts::TimeStamp>(atol((char*) csv_row[i].c_str())));
            } else {
                data1.emplace_back(atof((char*) csv_row[i].c_str()));
            }
        }
    } else if (portal_name.find("diagnostics_") != std::string::npos) {
        for (int i = 0; i < csv_row.size(); ++i) {
            if (i == 0) {
                data1.emplace_back(static_cast<kwdbts::TimeStamp>(atol((char*) csv_row[i].c_str())));
            } else if (i == 3) {
                data1.emplace_back(atol((char*) csv_row[i].c_str()));
            } else {
                data1.emplace_back(atof((char*) csv_row[i].c_str()));
            }
        }
    } else if (portal_name.find("mem_") != std::string::npos){
        for (int i = 0; i < csv_row.size(); ++i) {
            if (i == 0) {
                data1.emplace_back(static_cast<kwdbts::TimeStamp>(atol((char*) csv_row[i].c_str())));
            } else if (i >= 7) {
                data1.emplace_back(atof((char*) csv_row[i].c_str()));
            } else {
                data1.emplace_back(atol((char*) csv_row[i].c_str()));
            }
        }
    } else {
        for (int i = 0; i < csv_row.size(); ++i) {
            if (i == 0) {
                data1.emplace_back(static_cast<kwdbts::TimeStamp>(atol((char*) csv_row[i].c_str())));
            } else {
                data1.emplace_back(atol((char*) csv_row[i].c_str()));
            }
        }
    }




    kwdbts::ErrorCode errCode = device->createTuples(device->getDeviceName(), &tuples);
    if (tuples == nullptr) {
        std::cout << "failed create tuples, " << (gKWDBErr ? gKWDBErr->getError(errCode) : errCode) << "\n";
        return 1;
    }
    int nRows = 1;
    std::vector<std::vector<std::any>> dataRows_vec(nRows);

    for (int i = 0; i < nRows; ++i) {
        for (int j = 0; j < csv_row.size(); ++j) {
            dataRows_vec[i].push_back(data1[j]);
        }
    }

    // 使用Tuples对象缓存数据
    if (!tuples->addToData(dataRows_vec)) {
        std::cout << "Failed to addToData, because " << tuples->getErrorMessage() << "\n";
    } else {
        // 缓存成功之后可以将数据保存到设备
        //errCode = device->saveData(data, );
        errCode = device->saveData(tuples);
        if (errCode != "00000") {
            std::cout << "Failed to save data, " << (gKWDBErr ? gKWDBErr->getError(errCode) : errCode) << "\n";
        }
    }
    return 0;
}

int CloseKConn() {
    KWDBConn->close();
    return 0;
}

int test() {
    std::cout<<"a"<<std::endl;
    return 0;
}