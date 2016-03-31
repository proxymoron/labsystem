from flask import render_template, url_for, redirect, request, g

from app import app
import storage
from entities import CourseElement, create_element, checkPermissionForElement


@app.route("/courses")
def course_element_list():
    """Show the list of courses"""
    courses = map(lambda el: CourseElement(el), storage.listCourses())

    return render_template('elements/course/list.html', courses=courses)


@app.route("/courses/<course>/delete")
def course_element_delete(course):
    """Delete a course"""
    course = CourseElement(course)
    checkPermissionForElement(g.user, 'edit', course)

    course.delete()

    return redirect(url_for('course_element_list'))


@app.route("/courses/<course>/branches/<branch>")
def course_element_view(course, branch):
    """Show a course"""
    course = CourseElement(course, branch)
    checkPermissionForElement(g.user, 'view', course)

    return render_template('elements/course/view.html', element=course)


@app.route("/course/create")
def course_element_create():
    """Create a new course"""
    courseName = request.form['course']
    storage.createCourse(courseName)

    create_element(courseName, 'master', 'course', {'type': 'Course'})

    return redirect(url_for('course_element_view', course=courseName, branch='master'))
