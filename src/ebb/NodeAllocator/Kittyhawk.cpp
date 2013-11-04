/*
  EbbRT: Distributed, Elastic, Runtime
  Copyright (C) 2013 SESA Group, Boston University

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <cstdlib>
#include <climits>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "ebb/SharedRoot.hpp"
#include "ebb/NodeAllocator/Kittyhawk.hpp"
#include "ebb/EventManager/EventManager.hpp"

#include <glob.h>
#include <vector>
#include <string>

inline void rmglob(const std::string& pat){
  using namespace std;
  glob_t glob_result;
  glob(pat.c_str(),GLOB_TILDE,NULL,&glob_result);
  for(unsigned int i=0;i<glob_result.gl_pathc;++i){
    unlink(glob_result.gl_pathv[i]);
  }
  globfree(&glob_result);
}

ebbrt::EbbRoot *ebbrt::Kittyhawk::ConstructRoot() {
  return new SharedRoot<Kittyhawk>();
}

ebbrt::Kittyhawk::Kittyhawk(EbbId id) : NodeAllocator{ id } 
{
  khdir  = getenv("KHCTL_BASEDIR");
}


#include <iostream>

unsigned int ebbrt::Kittyhawk::Allocate(std::string app, std::string config,
                                        size_t num) {
  // compute random tag
  unsigned int tag = rand() % UINT_MAX;

  std::string loadcmd = "khget -i -d " + config + " na" + std::to_string(tag) +
                        " " + app + " " + std::to_string(num);
  popen(loadcmd.c_str(), "r");
  // FILE *fp;
  // char out[1024];

  // fp = popen(loadcmd.c_str(), "r");
  // while (fgets(out, 1024, fp) != NULL)
  //   printf("%s", out);
  // pclose(fp);

  return tag;
}

void ebbrt::Kittyhawk::Free(unsigned int tag) {
  std::string loadcmd = "khrm na" + std::to_string(tag);
  FILE *fp;
  char out[1024];

  fp = popen(loadcmd.c_str(), "r");
  while (fgets(out, 1024, fp) != NULL)
    printf("%s", out);
  pclose(fp);
}

void ebbrt::Kittyhawk::SetStatus(NetworkId node, std::string status) {
  if (khdir ) {
    char buf[160];
    int n=snprintf(buf, 169, "%s/khmonstate/%d *", khdir, node.addr & 0xff);
    int fd;
    
    if (n &&  n<160) rmglob(buf);
    
    n=snprintf(buf, 169, "%s/khmonstate/%d %s", khdir, node.addr & 0xff, status.c_str());
    if (n && n<160) {
      fd = open(buf, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
      if (fd) close(fd);
    }
  }
}
