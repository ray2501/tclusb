%{!?directory:%define directory /usr}

%define buildroot %{_tmppath}/%{name}

Name:          tclusb
Summary:       List USB devices in Tcl via libusb
Version:       0.5
Release:       0
License:       BSD
Group:         Development/Libraries/Tcl
Source:        %name-%version.tar.gz
URL:           https://github.com/ray2501/tclusb
Buildrequires: autoconf
Buildrequires: gcc
Buildrequires: libusb-1_0-devel
Buildrequires: tcl-devel >= 8.4
BuildRoot:     %{buildroot}

%description
List USB devices in Tcl (using libusb-1.0).

%prep
%setup -q -n %{name}-%version

%build
./configure \
	--prefix=%{directory} \
	--exec-prefix=%{directory} \
	--libdir=%{directory}/%{_lib}
make 

%install
make DESTDIR=%{buildroot} pkglibdir=%{tcl_archdir}/%{name}%{version} install

%clean
rm -rf %buildroot

%files
%defattr(-,root,root)
%{tcl_archdir}
