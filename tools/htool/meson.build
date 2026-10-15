
git_version_h = vcs_tag(
    command: ['git', 'describe', '--always', '--dirty', '--abbrev=40'],
    input: 'git_version.h.in',
    output: 'git_version.h',
    replace_string: '@GIT_COMMIT@',
)

incdir = libhoth_include_dirs
link_with = [libhoth.get_static_lib(), libhoth_cli]
c_args = []

if get_option('dbus_backend')
    link_with += libhoth_dbus
    c_args += '-DDBUS_BACKEND'
endif

# Everything in htool except main(). Link this from a custom main.c to add
# commands and transports via htool_run(); see htool.h.
htool_lib = static_library(
    'htool_lib',
    sources: [
        'htool.c',
        'htool_authz_command.c',
        'htool_console.c',
        'htool_dbus.c',
        'htool_debug.c',
        'htool_debug.h',
        'htool_dfu.c',
        'htool_firmware_update.c',
        'htool_i2c.c',
        'htool_jtag.c',
        'htool_mtd.c',
        'htool_key_rotation.c',
        'htool_mauv.h',
        'htool_mauv.c',
        'htool_panic.c',
        'htool_payload.c',
        'htool_payload_update.c',
        'htool_raw_host_command.c',
        'htool_rot_usb.c',
        'htool_run.c',
        'htool_spi.c',
        'htool_statistics.c',
        'htool_target_control.c',
        'htool_target_watchdog.c',
        'htool_target_watchdog.h',
        'htool_sbs_single.c',
        'htool_sbs_dual.c',
        'htool_usb.c',
        'htool_secure_boot.h',
        'htool_secure_boot.c',
        'htool_srtm.h',
        'htool_srtm.c',
        'htool_security_info.h',
        'htool_security_info.c',
        'htool_provisioning.h',
        'htool_provisioning.c',
        'htool_security_certificates.h',
        'htool_security_certificates.c',
        'htool_security_tokens.h',
        'htool_security_tokens.c',
        'htool_tpm.h',
        'htool_tpm.c',
        'htool_update_session.h',
        'htool_update_session.c',
    ],
    dependencies: [libusb],
    link_with: link_with,
    include_directories: incdir,
    c_args: c_args,
)

executable(
    'htool',
    sources: ['main.c', git_version_h],
    link_with: [htool_lib],
    include_directories: incdir,
    install: true,
)


gtest_dep = dependency('gtest', main: true, disabler: true, required: get_option('tests'))
gmock_dep = dependency('gmock', main: false, disabler: true, required: get_option('tests'))

if gtest_dep.found() and gmock_dep.found()
    test(
        'htool_run_test',
        executable(
            'htool_run_test_exe',
            sources: ['htool_run_test.cc'],
            include_directories: incdir,
            link_with: [htool_lib],
            dependencies: [gtest_dep, gmock_dep],
        ),
    )

    # These tests compile only their module's sources and supply the other
    # htool symbols they need themselves, so they must not link htool_lib.
    htool_module_test_srcs = [
        'test/test_util.h',
        'test/test_util.cc',
        '../../protocol/test/libhoth_device_mock.cc',
    ]
    htool_module_tests = {
        'htool_provisioning_test': ['htool_provisioning.c'],
        'htool_security_certificates_test': ['htool_security_certificates.c'],
        'htool_security_tokens_test': ['htool_security_tokens.c'],
    }
    foreach name, module_srcs : htool_module_tests
        foreach variant : ['', '_asan']
            sanitize = variant == '_asan' ? ['-fsanitize=address'] : []
            test(
                name + variant,
                executable(
                    name + variant + '_exe',
                    sources: htool_module_test_srcs + module_srcs + [name + '.cc'],
                    include_directories: incdir,
                    link_with: [libhoth.get_static_lib()],
                    dependencies: [gtest_dep, gmock_dep],
                    c_args: sanitize,
                    cpp_args: sanitize,
                    link_args: sanitize,
                ),
            )
        endforeach
    endforeach
endif

