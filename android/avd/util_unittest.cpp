// Copyright 2014 The Android Open Source Project
//
// This software is licensed under the terms of the GNU General Public
// License version 2, as published by the Free Software Foundation, and
// may be copied, distributed, and modified under those terms.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

#include "android/avd/util.h"
#include "android/utils/file_data.h"

#include <gtest/gtest.h>

TEST(AvdUtil, emulator_getBackendSuffix) {
  EXPECT_STREQ("arm", emulator_getBackendSuffix("arm"));
  EXPECT_STREQ("x86", emulator_getBackendSuffix("x86"));
  EXPECT_STREQ("x86", emulator_getBackendSuffix("x86_64"));
  EXPECT_STREQ("mips", emulator_getBackendSuffix("mips"));

  // TODO(digit): Add support for these CPU architectures to the emulator
  // to change these to EXPECT_STREQ() calls.
  EXPECT_FALSE(emulator_getBackendSuffix("arm64"));
  EXPECT_FALSE(emulator_getBackendSuffix("mips64"));

  EXPECT_FALSE(emulator_getBackendSuffix(NULL));
  EXPECT_FALSE(emulator_getBackendSuffix("dummy"));
}

TEST(AvdUtil, propertyFile_getInt) {
  FileData fd;

  const char* testFile =
    "nineteen=19\n"
    "int_min=-2147483648\n"
    "int_max=2147483647\n"
    "invalid=2147483648\n"
    "invalid2=-2147483649\n"
    "invalid3=bar\n"
    "empty=\n";

  EXPECT_EQ(0,fileData_initFromMemory(&fd, testFile, strlen(testFile)));

  const int kDefault = 1138;
  SearchResult kSearchResultGarbage = (SearchResult)0xdeadbeef;
  SearchResult searchResult = kSearchResultGarbage;

  EXPECT_EQ(kDefault,propertyFile_getInt(&fd, "invalid", kDefault, &searchResult));
  EXPECT_EQ(RESULT_INVALID,searchResult);

  searchResult = kSearchResultGarbage;
  EXPECT_EQ(kDefault,propertyFile_getInt(&fd, "invalid2", kDefault, &searchResult));
  EXPECT_EQ(RESULT_INVALID,searchResult);

  searchResult = kSearchResultGarbage;
  EXPECT_EQ(kDefault,propertyFile_getInt(&fd, "invalid3", kDefault, &searchResult));
  EXPECT_EQ(RESULT_INVALID,searchResult);

  searchResult = kSearchResultGarbage;
  EXPECT_EQ(kDefault,propertyFile_getInt(&fd, "bar", kDefault, &searchResult));
  EXPECT_EQ(RESULT_NOT_FOUND,searchResult);

  searchResult = kSearchResultGarbage;
  EXPECT_EQ(kDefault,propertyFile_getInt(&fd, "empty", kDefault, &searchResult));
  EXPECT_EQ(RESULT_INVALID,searchResult);

  searchResult = kSearchResultGarbage;
  EXPECT_EQ(19,propertyFile_getInt(&fd, "nineteen", kDefault, &searchResult));
  EXPECT_EQ(RESULT_FOUND,searchResult);

  // check that null "searchResult" parameter is supported
  EXPECT_EQ(kDefault,propertyFile_getInt(&fd, "bar", kDefault, NULL));
  EXPECT_EQ(kDefault,propertyFile_getInt(&fd, "invalid", kDefault, NULL));
  EXPECT_EQ(19,propertyFile_getInt(&fd, "nineteen", kDefault, NULL));
}

TEST(AvdUtil, propertyFile_getApiLevel) {
  FileData fd;

  const char* emptyFile =
    "\n";

  const char* testFile19 =
    "ro.build.version.sdk=19\n";

  const char* testFileBogus =
    "ro.build.version.sdk=bogus\n";

  EXPECT_EQ(0,fileData_initFromMemory(&fd, emptyFile, strlen(emptyFile)));
  EXPECT_EQ(10000,propertyFile_getApiLevel(&fd));

  EXPECT_EQ(0,fileData_initFromMemory(&fd, testFile19, strlen(testFile19)));
  EXPECT_EQ(19,propertyFile_getApiLevel(&fd));

  EXPECT_EQ(0,fileData_initFromMemory(&fd, testFileBogus, strlen(testFileBogus)));
  EXPECT_EQ(3,propertyFile_getApiLevel(&fd));
}

TEST(AvdUtil, propertyFile_getAdbdCommunicationMode) {
  FileData fd;

  const char* emptyFile =
    "\n";

  const char* testFile15 =
    "ro.build.version.sdk=15\n";

  const char* testFile16 =
    "ro.build.version.sdk=16\n";

  const char* testFile15_0 =
    "ro.build.version.sdk=15\n"
    "ro.adb.qemud=0";

  const char* testFile15_1 =
    "ro.build.version.sdk=15\n"
    "ro.adb.qemud=1";

  const char* testFile16_0 =
    "ro.build.version.sdk=16\n"
    "ro.adb.qemud=0";

  const char* testFile16_1 =
    "ro.build.version.sdk=16\n"
    "ro.adb.qemud=1";

  // API unspecified -> API level == 10000
  EXPECT_EQ(0,fileData_initFromMemory(&fd, emptyFile, strlen(emptyFile)));
  EXPECT_EQ(1,propertyFile_getAdbdCommunicationMode(&fd));

  EXPECT_EQ(0,fileData_initFromMemory(&fd, testFile15, strlen(testFile15)));
  EXPECT_EQ(0,propertyFile_getAdbdCommunicationMode(&fd));

  EXPECT_EQ(0,fileData_initFromMemory(&fd, testFile16, strlen(testFile16)));
  EXPECT_EQ(1,propertyFile_getAdbdCommunicationMode(&fd));

  EXPECT_EQ(0,fileData_initFromMemory(&fd, testFile15_0, strlen(testFile15_0)));
  EXPECT_EQ(0,propertyFile_getAdbdCommunicationMode(&fd));

  EXPECT_EQ(0,fileData_initFromMemory(&fd, testFile15_1, strlen(testFile15_1)));
  EXPECT_EQ(0,propertyFile_getAdbdCommunicationMode(&fd));

  EXPECT_EQ(0,fileData_initFromMemory(&fd, testFile16_0, strlen(testFile16_0)));
  EXPECT_EQ(0,propertyFile_getAdbdCommunicationMode(&fd));

  EXPECT_EQ(0,fileData_initFromMemory(&fd, testFile16_1, strlen(testFile16_1)));
  EXPECT_EQ(1,propertyFile_getAdbdCommunicationMode(&fd));
}

