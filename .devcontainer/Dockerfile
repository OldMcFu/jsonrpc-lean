FROM registry.access.redhat.com/ubi9/ubi 
RUN dnf update -y
RUN dnf upgrade -y
RUN dnf install https://dl.fedoraproject.org/pub/epel/epel-release-latest-9.noarch.rpm -y
RUN dnf install python3 git -y

RUN dnf install gprbuild make cmake gcc g++ gcc-gnat wget -y